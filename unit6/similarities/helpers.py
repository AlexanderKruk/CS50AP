def lines(a, b):
    """Return lines in both a and b"""
    # convert to set
    a, b = set(a.split('\n')), set(b.split('\n'))

    # list identically line for a and b
    iden_line = []

    # compare word in two sets
    for word in a:
        if word in b:
            iden_line.append(word)

    return iden_line


def sentences(a, b):
    """Return sentences in both a and b"""
    from nltk.tokenize import sent_tokenize

    # list identically sentence for a and b
    iden_sent = []

    a, b = set(sent_tokenize(a)), set(sent_tokenize(b))

    # compare sentence in two sets
    for sentence in a:
        if sentence in b:
            iden_sent.append(sentence)

    return iden_sent


def substr(str, n):
    "Return list of substring without dublicates of length n for string"
    substr = []

    # cut string for substrings
    for i in range(len(str)):
        if str[i:i + n] not in substr and len(str[i:i + n]) > n - 1:
            substr.append(str[i:i + n])

    return substr


def substrings(a, b, n):
    """Return substrings of length n in both a and b"""
    # list substrings without dublicates with n lenght
    a, b = substr(a, n), substr(b, n)

    # list edentically substring for a and b
    sub = []

    # compare substring in a and b
    for substring in a:
        if substring in b:
            sub.append(substring)

    return sub
