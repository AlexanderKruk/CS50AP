from cs50 import get_float

# check input more then 0 and number
while True:
    dollar = get_float("Change owed: ")
    if dollar > 0:
        break

# convert to cent
cent = dollar * 100

# count coins
coin25 = int(cent / 25)
coin10 = int((cent - coin25 * 25) / 10)
coin5 = int((cent - (coin25 * 25 + coin10 * 10)) / 5)
coin1 = int((cent - (coin25 * 25 + coin10 * 10 + coin5 * 5)) / 1)

print(coin25 + coin10 + coin5 + coin1)
