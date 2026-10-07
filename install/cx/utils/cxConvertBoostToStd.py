#!/usr/bin/env python3
'''
Replace Boost smart pointers, function wrappers, arrays, fixed-width integers
and unordered_map with their std equivalents in C++ source files (CustusS#437).

Only replacements where the std type behaves the same as the Boost type are
done, so the result is checked by the compiler. Other Boost usage (bind, math,
lexical_cast, ...) is left unchanged.

Run on a source tree (or single files), e.g. after merging develop into a
branch that still adds Boost pointer code:
    python3 install/cx/utils/cxConvertBoostToStd.py source
'''
import argparse
import os
import re
import sys

CPP_EXTENSIONS = ('.h', '.hpp', '.hxx', '.txx', '.cpp', '.cxx', '.cc', '.mm')

IDENTIFIER_REPLACEMENTS = [
    ('shared_ptr', 'shared_ptr'),
    ('weak_ptr', 'weak_ptr'),
    ('enable_shared_from_this', 'enable_shared_from_this'),
    ('dynamic_pointer_cast', 'dynamic_pointer_cast'),
    ('static_pointer_cast', 'static_pointer_cast'),
    ('const_pointer_cast', 'const_pointer_cast'),
    ('make_shared', 'make_shared'),
    ('scoped_ptr', 'unique_ptr'),
    ('function', 'function'),
    ('array', 'array'),
    ('unordered_map', 'unordered_map'),
    ('uint8_t', 'uint8_t'),
    ('uint16_t', 'uint16_t'),
    ('uint32_t', 'uint32_t'),
    ('uint64_t', 'uint64_t'),
    ('int8_t', 'int8_t'),
    ('int16_t', 'int16_t'),
    ('int32_t', 'int32_t'),
    ('int64_t', 'int64_t'),
]

INCLUDE_REPLACEMENTS = {
    'boost/shared_ptr.hpp': 'memory',
    'boost/weak_ptr.hpp': 'memory',
    'boost/make_shared.hpp': 'memory',
    'boost/scoped_ptr.hpp': 'memory',
    'boost/enable_shared_from_this.hpp': 'memory',
    'boost/function.hpp': 'functional',
    'boost/array.hpp': 'array',
    'boost/cstdint.hpp': 'cstdint',
    'boost/unordered_map.hpp': 'unordered_map',
}

STD_HEADER_USAGE = [
    ('memory', r'\bstd::(shared_ptr|weak_ptr|unique_ptr|enable_shared_from_this|make_shared|dynamic_pointer_cast|static_pointer_cast|const_pointer_cast)\b'),
    ('functional', r'\bstd::function\b'),
    ('array', r'\bstd::array\b'),
    ('unordered_map', r'\bstd::unordered_map\b'),
    ('cstdint', r'\bstd::u?int(8|16|32|64)_t\b'),
]

INCLUDE_PATTERN = re.compile(r'^([ \t]*)#[ \t]*include[ \t]*[<"]([^>"]+)[>"][^\n]*$', re.MULTILINE)


def replaceIdentifiers(text):
    '''Replace boost::X with std::Y for every known X.'''
    for boostName, stdName in IDENTIFIER_REPLACEMENTS:
        text = re.sub(r'\bboost::%s\b' % boostName, 'std::%s' % stdName, text)
    return text


def replaceIncludes(text):
    '''Replace known Boost headers with the std header, without duplicating includes.'''
    included = set(match.group(2) for match in INCLUDE_PATTERN.finditer(text))

    def replace(match):
        header = match.group(2)
        if header not in INCLUDE_REPLACEMENTS:
            return match.group(0)
        stdHeader = INCLUDE_REPLACEMENTS[header]
        if stdHeader in included:
            return None
        included.add(stdHeader)
        lineEnd = '\r' if match.group(0).endswith('\r') else ''
        return '%s#include <%s>%s' % (match.group(1), stdHeader, lineEnd)

    lines = []
    for line in text.split('\n'):
        match = INCLUDE_PATTERN.match(line)
        if match:
            line = replace(match)
        if line is not None:
            lines.append(line)
    return '\n'.join(lines)


def findIncludeInsertLine(lines):
    '''Return the line index to insert a new include at: after the first include, else after the include guard.'''
    for i, line in enumerate(lines):
        if INCLUDE_PATTERN.match(line):
            return i + 1
    for i, line in enumerate(lines):
        if re.match(r'^[ \t]*#[ \t]*define\b', line) and i > 0 and re.match(r'^[ \t]*#[ \t]*ifndef\b', lines[i - 1]):
            return i + 1
    return 0


def addMissingIncludes(text):
    '''Include the std header for every std type used, as Boost headers may have provided it indirectly.'''
    included = set(match.group(2) for match in INCLUDE_PATTERN.finditer(text))
    missing = []
    for header, pattern in STD_HEADER_USAGE:
        if header not in included and header not in missing and re.search(pattern, text):
            missing.append(header)
    if not missing:
        return text
    lines = text.split('\n')
    lineEnd = '\r' if lines[0].endswith('\r') else ''
    index = findIncludeInsertLine(lines)
    lines[index:index] = ['#include <%s>%s' % (header, lineEnd) for header in missing]
    return '\n'.join(lines)


def convertText(text):
    converted = replaceIdentifiers(text)
    identifiersReplaced = converted != text
    converted = replaceIncludes(converted)
    if identifiersReplaced:
        converted = addMissingIncludes(converted)
    return converted


def convertFile(path):
    '''Convert one file in place. Return True if it changed.'''
    with open(path, 'r', encoding='utf-8', errors='surrogateescape', newline='') as f:
        original = f.read()
    converted = convertText(original)
    changed = converted != original
    if changed:
        with open(path, 'w', encoding='utf-8', errors='surrogateescape', newline='') as f:
            f.write(converted)
    return changed


def findFiles(paths):
    for path in paths:
        if os.path.isfile(path):
            yield path
            continue
        for root, dirs, files in os.walk(path):
            dirs[:] = [d for d in dirs if d != '.git']
            for name in files:
                if name.endswith(CPP_EXTENSIONS):
                    yield os.path.join(root, name)


def main(argv):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('paths', nargs='+', help='Source folders or files to convert in place')
    args = parser.parse_args(argv)
    count = 0
    for path in findFiles(args.paths):
        if convertFile(path):
            count += 1
    print('Converted %d files' % count)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
