from cs50 import get_string
from sys import argv

# check arguments
if len(argv) != 2:
    print("Usage python caesar.py k")

# offset
k = int(argv[1])

# get string for cipher
text = get_string("plaintext: ")

print("ciphertext: ", end="")

# make cipher
for ch in text:
    if ch.isalpha():
        if ch.islower():
            print(chr(((ord(ch) - 97 + k) % 26) + 97), end="")
        elif ch.isupper():
            print(chr(((ord(ch) - 65 + k) % 26) + 65), end="")
    else:
        print(ch, end="")
print()

