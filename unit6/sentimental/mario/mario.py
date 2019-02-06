from cs50 import get_int

# check input value for 0-23
while True:
    height = get_int("height: ")
    if height >= 0 and height <= 23:
        break

# draw pyramide
for i in range(height):
    print(" "*(height - (i + 1)), end="")
    print("#"*(i + 1), end="")
    print("  ", end="")
    print("#"*(i + 1), end="")
    print()