#!/usr/bin/env python
"""
gem.files handles opening files and streams
"""
from itertools import islice

import subprocess
import __builtin__
import gem


__author__ = 'Thasso Griebel'
__zcat_path = None

class Parser(object):
    def __init__(self):
        self.read = gem.Read()

    def next(self, stream):
        """Implement this method
        and parse the next entry from the
        stream

        @param stream: the input stream
        @type stream: stream
        """
        pass


class parse_fasta(Parser):
    """Parse fasta entries from a stream
    """
    def next(self, stream):
        fasta_lines = list(islice(stream, 2))  # read in chunks of 2
        if not fasta_lines:
            return None

        self.read.id = fasta_lines[0].rstrip()[1:]
        self.read.sequence = fasta_lines[1].rstrip()
        self.read.qualities = None
        self.read.summary = None
        self.read.mappings = None

        if len(read.sequence) <= 0:
            return self.next(stream)
        return self.read


class parse_fastq(Parser):
    """Parse fastq entries from a stream"""
    def next(self, stream):
        fastq_lines = list(islice(stream, 4))  # read in chunks of 2
        if not fastq_lines:
            return None

        self.read.id = fastq_lines[0].rstrip()[1:]
        self.read.sequence = fastq_lines[1].rstrip()
        self.read.qualities = fastq_lines[3].rstrip()
        self.read.summary = None
        self.read.mappings = None

        if len(self.read.sequence) <= 0:
            return self.next(stream)
        return self.read


class parse_map(Parser):
    """Parse gem map entries from a stream"""
    def next(self, stream):
        line = stream.readline()
        if not line:
            return None
        line = line.rstrip()
        split = line.split("\t")
        self.read.id = split[0]
        self.read.sequence = split[1]
        if len(split) == 5:
            # read with qualities
            self.read.qualities = split[2]
            self.read.summary = split[3]
            self.read.mappings = split[4]
        else:
            # read with no qualities
            self.read.summary = split[2]
            self.read.mappings = split[3]
            self.read.qualities = None
        if self.read.summary in ["+", "-", "*"]:
            self.read.summary = "0"
        return self.read



class ReadIterator(object):
    def __init__(self, stream, parser, filename=None, process=None):
        """
        Create a ReadIterator from a stream with a given parser.
        If the filename is given, the iterator can be cloned to re-read
        the file. Cloning the iterator will not change the curent state
        of this instance. The returned clone will start again from the
        beginning of the file.

        @param stream: the input stream
        @type stream: stream
        @param parser: the read parser
        @type type: Parser
        @param filename: optional name of the input file
        @type filename: string
        @param process: optinal process associated with this reader
        @type process: process
        """
        self.stream = stream
        self.parser = parser
        self.filename = filename
        self.process = process

    def __iter__(self):
        return self

    def next(self):
        """ Delegates to the parser
        to get the next entry
        """
        ret = self.parser.next(self.stream)
        if not ret:
            self.stream.close()
            raise StopIteration()
        return ret

    def close(self):
        self.stream.close()

    def clone(self):
        if not self.filename:
            raise ValueError("No filename given, this reader can not be cloned!")
        return ReadIterator(open_file(self.filename), self.parser.__class__(), self.filename)


## type to parser map
supported_types = {
    "fasta": parse_fasta,
    "fastq": parse_fastq,
    "map": parse_map
}



def open(input, type=None, process=None):
    """
    Open the given file and return on iterator
    over Reads.

    The file parameter has to be a string.

    @param input: string of the file name or an open stream
    @type input: string or stream
    @param type: the type of the input file or none for auto-detection
    @type type: string
    @param process: optional process associated with the input
    @type process: Process
    """
    is_string  = isinstance(input, basestring)
    if type is None and is_string:
        type = _guess_type(input)
        if type is None:
            type = "map"
    if type not in supported_types.keys():
        raise ValueError("Unknown file type %s, call this method with a specific type (%s)" % (input, supported_types))

    stream = input
    if is_string:
        stream = open_file(input)
    else:
        input = None  ## reset filename
    return ReadIterator(stream, supported_types[type](), input, process=process)


def open_file(file):
    """
    Open the given file and return a stream
    of the file content. The method checks if the file
    ends with .gz and open a gzip stream in that case.

    The file parameter has to be a string.

    @param file: string of the file name
    @type file: string
    """
    if file.endswith(".gz"):
        return open_gzip(file)
    else:
        return __builtin__.open(file, 'r')


def _guess_type(name):
    """
    guess the type based on the given file name
    and returns one of teh following:

    fasta, fastq, map

    or None if no type could be detected

    @param name: the name of the file
    @type name: string
    @return: one of fasta, fastq, map or None
    @rtype: string
    """
    name = name.upper()
    if name.endswith(".GZ"):
        name = name[:-3]
    if name.endswith(".FASTA") or name.endswith("FA"):
        return "fasta"
    elif name.endswith(".FASTQ"):
        return "fastq"
    elif name.endswith(".MAP"):
        return "map"
    return None


def open_gzip(file_name):
    """Uses a separate zcat process to open a gzip
    stream.

    @param file_name: the name of the input file
    @return stream: gzip stream
    @rtype stream
    """
    if not isinstance(file_name, basestring):
        raise ValueError("The provided file name is not a string : %s" % (file_name))
    zcat = subprocess.Popen([__zcat(), file_name], stdout=subprocess.PIPE)
    return zcat.stdout


def __zcat():
    """
    get the path to the zcat/gzcat executable
    or raise an exception
    """
    global __zcat_path
    if __zcat_path is not None:
        return __zcat_path
    __zcat_path = __which("zcat")
    if not __zcat_path:
        __zcat_path = __which("gzcat")
    if not __zcat_path:
        raise ValueError("Unable to find a zcat|gzcat executable in PATH!")
    return __zcat_path


def __which(program):
    """
    Find programm in path
    """
    import os
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None
