#!/usr/bin/env python3
"""
spearmans_footrule.py

Computes the Spearman's footrule distance between the current and the ordered sets.
The Spearman's footrule distance is a non-negative measure of the difference 
or disagreement of two ordered lists, each of which is a full permutation of a set.

Usage: python spearmans_footrule.py <log_file> timestamp_index [--quiet|--verbose max_num]
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

def abort():
    print("Usage: python spearmans_footrule.py <log_file> timestamp_index [--quiet|--verbose max_num]")
    sys.exit(1)

if __name__ != "__main__":
    abort()

if len(sys.argv) < 3 or len(sys.argv) > 5:
    abort()

timestamp_index = 0
try:
    timestamp_index = int(sys.argv[2])
except ValueError:
    abort()
  
quiet_mode = False
verbose = False
max_verbose = -1
if len(sys.argv) >= 4:
    if sys.argv[3] == '--quiet':
        quiet_mode = True
        verbose = False
    elif sys.argv[3] == '--verbose':
        quiet_mode = False
        verbose = True
        if len(sys.argv) == 5:
            try:
                max_verbose = int(sys.argv[4])
            except ValueError:
                abort()  
    else:
        abort()
 
"""
Processes the log file line-by-line.
"""
footrule_distance = 0
line_count = 0

filename = sys.argv[1]

if verbose:   
    print(f"Processing file: {filename} ...")

file_encoding = detect_encoding(filename)

with open(filename, 'r', encoding=file_encoding) as f:
    data = []
    
    for line in f:
        line_count += 1
        # Extract time
        fields = line.strip().split()
        if len(fields) < timestamp_index:
            sys.exit("Error: Log was malformed!")
        
        time_str = fields[timestamp_index - 1]
        if verbose and line_count < 2:
            print(f"\rTimestamp field: {time_str}")
        time = 0
        try:
            time = int(time_str)
        except ValueError:
            try:
                time = parse_time_to_nanoseconds(time_str)
            except (ValueError, IndexError):
                sys.exit("Error: Timestamp was malformed!")
        
        data.append([time, line_count])
     
        # Progress update every 100,000 lines
        if not quiet_mode and line_count % 100000 == 0:
            print(f"\rParsed {line_count} lines...", end="")
    if not quiet_mode :
        print(f"\rParsed {line_count} lines...")
       
    data.sort(key=lambda row: row[0])
    
    line_count = 0
    for row in data:
        line_count += 1
        error = abs(line_count - row[1])
        footrule_distance += error
        if verbose and max_verbose != 0 and error != 0:
           print(f"The line {row[1]} should be in place {line_count}")
           max_verbose -= 1
       
if quiet_mode :    
    print(footrule_distance)
else:
    print("\n==================================================")
    print(f"Spearman's footrule distance: {footrule_distance}")
