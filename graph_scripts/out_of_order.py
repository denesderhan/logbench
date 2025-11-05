#!/usr/bin/env python3
"""
out_of_order.py

Counts lines whose TIME stamp is smaller than the previous line's time.
Only the TIME part is used (date is ignored).

Usage:
    python out_of_order.py <log_file.txt> [--quiet]
"""

import sys
import chardet

def parse_time_to_nanoseconds(time_str: str):
    """
    Efficiently converts time string (HH:MM:SS.ffffff) to nanoseconds since midnight.
    """
    if '.' in time_str:
        main, frac = time_str.split('.', 1)
    elif ',' in time_str:
        main, frac = time_str.split(',', 1)
    else:
        main = time_str
        frac = '0'
    frac = frac.ljust(9, '0')[:9]  # Ensure 9 digits, pad with zeros if needed, truncate if longer
        
    if ':' in main:
        parts = main.split(':')
    else:
       raise ValueError(f"Invalid time format: {time_str}") 
    
    if len(parts) != 3:
        raise ValueError(f"Invalid time format: {time_str}")
        
    try:
        hh, mm, ss = map(int, parts)
        nano = int(frac)
    except ValueError:
        raise ValueError(f"Invalid time format: {time_str}")

    return hh * 3_600_000_000_000 + mm * 60_000_000_000 + ss * 1_000_000_000 + nano

def detect_encoding(file_path, chunk_size=1024):
    with open(file_path, 'rb') as f:
        # Read a small chunk (enough for detection)
        raw_data = f.read(chunk_size)
        result = chardet.detect(raw_data)
        return result['encoding']

def main(filename: str, quiet=False):
    """
    Processes the log file line-by-line. 
    'quiet' mode disables logging individual out-of-order lines.
    """
    prev_nano = 0
    bad_block_count = 0
    line_count = 0
    max_nano = 0
    bad_block_start = 0
    bad_block_lines = 0
    regression_count = 0
    max_block1_lines = 0
    max_block1_nano = 0
    max_block2_lines = 0
    max_block2_nano = 0
        
    print(f"Processing file: {filename} ...")
    
    file_encoding = detect_encoding(filename)

    with open(filename, 'r', encoding=file_encoding) as f:
        for line in f:
            line_count += 1

            # Extract time (2nd field)
            fields = line.strip().split()
            if len(fields) < 2:
                sys.exit("Error: Log was malformed!")

            try:
                current_nano = parse_time_to_nanoseconds(fields[1])
            except (ValueError, IndexError):
                sys.exit("Error: Timestamp was malformed!")
                       
            """
            A bad block is a continuous region of time-sorted events that are out of order globally.
            They are not necessarily regressions from line to line.
            The timestamps within the block may be in order, or even decreasing.
            But the entire block is misplaced because it falls before the correct chronological position.

            Once we detect that we're in a "bad block" (i.e., we've seen a timestamp smaller than max_nano),
            then we remain inside the block until we find a timestamp that is greater than all previous timestamps.
            """            
            
            if current_nano < max_nano:
                bad_block_lines += 1   
           
            if current_nano < prev_nano:
                if verbose:
                        print(f"Line {line_count}: Time regression detected! prev:{prev_nano}, current:{current_nano}")
                regression_count += 1
                # start new bad block (if regressed and not already in a bad block)
                if bad_block_start == 0:
                    bad_block_start = current_nano
                    bad_block_count += 1
                    if not quiet:
                        print(f"Line {line_count}: Bad block start detected")
            elif current_nano >= max_nano:
                # end the bad block (chronological order restored)
                if bad_block_start != 0:
                    block_nano = current_nano - bad_block_start
                    if block_nano > max_block1_nano:
                        max_block1_nano = block_nano
                        max_block1_lines = bad_block_lines
                    if bad_block_lines > max_block2_lines:
                        max_block2_nano = block_nano
                        max_block2_lines = bad_block_lines
                    if not quiet:
                        print(f"Bad block length in nano: {block_nano}, in lines: {bad_block_lines}")
                    bad_block_start = 0
                    bad_block_lines = 0
                max_nano = current_nano

            prev_nano = current_nano

            # Progress update every 100,000 lines
            if line_count % 100000 == 0:
                print(f"\rProcessed {line_count} lines...", end="")

    print("\n==================================================")
    if bad_block_count != 0:
        print(f"Local regression count: {regression_count}")
        print(f"Out-of-order blocks: {bad_block_count}")
        print(f"Longest bad block (nanosec): nanosec {max_block1_nano}, lines: {max_block1_lines}")
        print(f"Longest bad block (lines): nanosec {max_block2_nano}, lines: {max_block2_lines}")
    else:
        print(f"No timestamp regressions detected!")


if __name__ == "__main__":
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Usage: python out_of_order.py <log_file.txt> [--quiet|--verbose]")
        sys.exit(1)

    quiet_mode = False
    verbose = False
    if len(sys.argv) == 3:
        if sys.argv[2] == '--quiet':
            quiet_mode = True
            verbose = False
        if sys.argv[2] == '--verbose':
            quiet_mode = False
            verbose = True
    main(sys.argv[1], quiet=quiet_mode)
