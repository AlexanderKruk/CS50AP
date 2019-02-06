import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions
from werkzeug.security import check_password_hash, generate_password_hash
from time import gmtime, strftime
from time import sleep

from helpers import apology, login_required, lookup, usd

# Ensure environment variable is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached


@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")


@app.route("/", methods=["GET", "POST"])
@login_required
def index():
    """Show portfolio of stocks"""

    if request.method == "POST":
        # checks for empty form
        if not request.form.get("stock"):
            return apology("Missing stock!", 400)
        elif not request.form.get("num_stocks"):
            return apology("Missing number of stocks!", 400)
        # check is not alpha in stocks name input
        elif not request.form.get("stock").isalpha():
            return apology("Stock name not correct!", 400)
        # check for positive integer in num_stocks input
        elif int(request.form.get("num_stocks")) <= 0:
            return apology("Number stocks must be positive", 400)
        elif request.form['submit_button'] == 'sell':
            # check user cash
            cash = db.execute("SELECT cash FROM 'users' WHERE id = :user_id", user_id=session["user_id"])

            # if stock not empty
            stocks = lookup(request.form.get("stock"))

            # if stocks api don't return any result
            if stocks == None:
                return apology("This stock is no exist", 400)

            # check if user have this stocks
            stock_portfolio = db.execute("SELECT * FROM 'portfolio' WHERE id_user = :user_id AND symbol = :symbol",
                                         user_id=session["user_id"], symbol=request.form.get("stock"))

            # check if stocks enough
            if len(stock_portfolio) > 0:
                if stock_portfolio[0]["num_stocks"] >= int(request.form.get("num_stocks")):
                    # update count of stocks in portfolio
                    stock_portfolio = db.execute("UPDATE 'portfolio' SET 'num_stocks' = :num_stocks WHERE id_user = :user_id AND symbol = :symbol", num_stocks=(
                        stock_portfolio[0]["num_stocks"] - int(request.form.get("num_stocks"))), user_id=session["user_id"], symbol=request.form.get("stock"))
                    # add sell transaction to transaction table
                    transaction = db.execute("INSERT INTO 'transaction' (symbol, price, num_stocks, action, date, id_user) VALUES (:symbol, :price, :num_stocks, :action, :datetime, :id_user)",
                                             symbol=request.form.get("stock"), price=stocks["price"], num_stocks=request.form.get("num_stocks"), action="sell", datetime=strftime("%Y-%m-%d %H:%M:%S", gmtime()), id_user=session["user_id"])
                    # update cash in user table
                    cash = db.execute("UPDATE 'users' SET 'cash' = :cash WHERE id = :user_id",
                                      cash=cash[0]['cash'] + (int(request.form.get("num_stocks")) * stocks["price"]), user_id=session["user_id"])
                else:
                    return apology("Num of stocks not enought", 400)
            else:
                return apology("User don't have this stocks", 400)

        elif request.form['submit_button'] == 'buy':
            # try to get stock price
            stock = lookup(request.form.get("stock"))
            num_stock = int(request.form.get("num_stocks"))

            # if stocks api don't return any result
            if stock == None:
                return apology("This stosk is no exist", 400)

            # look at db for user cash
            cash = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=session["user_id"])

            # if cash is enough buy stocks
            if len(cash) > 0:
                if cash[0]["cash"] > stock["price"] * num_stock:
                    buy_stocks = db.execute("INSERT INTO 'transaction' (symbol, price, num_stocks, action, date, id_user) VALUES (:symbol, ':price', :num_stocks, :action, :datetime, ':id_user')",
                                            symbol=stock["symbol"], price=stock["price"], num_stocks=request.form.get("num_stocks"), action="buy", datetime=strftime("%Y-%m-%d %H:%M:%S", gmtime()), id_user=session["user_id"])

                    # messege about successful buy
                    if buy_stocks is not None:
                        message = str(num_stock) + " stocks " + str(stock["symbol"]) + \
                            " is successfully purchased at price " + str(stock["price"])

                    # update user cash
                    cash = db.execute("UPDATE 'users' SET cash = :cash WHERE id = :id_user",
                                      cash=cash[0]["cash"] - stock["price"] * num_stock, id_user=session["user_id"])

                    portfolio_stocks = db.execute(
                        "SELECT * FROM 'portfolio' WHERE id_user = :id_user AND symbol = :symbol", id_user=session["user_id"], symbol=stock["symbol"])

                    # insert or update portfolio
                    if len(portfolio_stocks) == 0:
                        portfolio_stocks_insrt = db.execute("INSERT INTO 'portfolio' (symbol, num_stocks, id_user) VALUES (:symbol, :num_stocks, :id_user)",
                                                            symbol=stock["symbol"], num_stocks=request.form.get("num_stocks"), id_user=session["user_id"])
                    elif len(portfolio_stocks) >= 1:
                        portfolio_stocks_update = db.execute("UPDATE 'portfolio' SET num_stocks = :num_stocks WHERE id = :id_portfolio",
                                                             num_stocks=portfolio_stocks[0]["num_stocks"] + num_stock, id_portfolio=portfolio_stocks[0]["id"])
                else:
                    return apology("Money not enough", 400)

    # get all stocks of current user
    stocks = db.execute("SELECT * FROM 'portfolio' WHERE id_user = :user_id", user_id=session["user_id"])

    # initialisation variable for all money in stocks
    all_stocks = 0

    # check user cash
    cash = db.execute("SELECT cash FROM 'users' WHERE id = :user_id", user_id=session["user_id"])

    num = 0

    # check current price of stock
    if len(stocks) > 0:
        for stock in stocks:
            info = lookup(stock["symbol"])
            num += 1
            stock.update({'cur_price': 0, 'cost': 0, 'number': num})
            if info != None:
                stock["cur_price"] = info["price"]
                stock["cost"] = stock["cur_price"] * stock["num_stocks"]
            all_stocks += stock["cost"]

        return render_template("index.html", stocks=stocks, all_stocks=all_stocks, cash=cash[0]["cash"], all_cash=cash[0]["cash"] + all_stocks)
    return render_template("index.html", stocks=stocks, all_stocks=0.0, cash=cash[0]["cash"], all_cash=cash[0]["cash"])


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    message = ""

    # User click on Buy button
    if request.method == "POST":

        # Check text field is empty
        if not request.form.get("symbol"):
            return apology("Missing share", 400)
        elif not request.form.get("shares"):
            return apology("Missing num stocks", 400)
        # check is not alpha in stocks name input
        elif not request.form.get("symbol").isalpha():
            return apology("Stock name not correct!", 400)
        # check if not float number of stocks
        elif request.form.get("shares").find(".") != -1:
            return apology("Number stocks must be int", 400)
        # check in not alpha number of stocks
        elif request.form.get("shares").isalpha():
            return apology("Number stocks must be int", 400)
        # check for positive integer in num_stocks input
        elif int(request.form.get("shares")) <= 0:
            return apology("Number stocks must be positive", 400)

        # try to get stock price
        stock = lookup(request.form.get("symbol"))
        num_stock = int(request.form.get("shares"))

        # if stocks api don't return any result
        if stock == None:
            return apology("This stosk is no exist", 400)

        # look at db for user cash
        cash = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=session["user_id"])

        # if cash is enough buy stocks
        if len(cash) > 0:
            if cash[0]["cash"] > stock["price"] * num_stock:
                buy_stocks = db.execute("INSERT INTO 'transaction' (symbol, price, num_stocks, action, date, id_user) VALUES (:symbol, ':price', :num_stocks, :action, :datetime, ':id_user')",
                                        symbol=stock["symbol"], price=stock["price"], num_stocks=request.form.get("shares"), action="buy", datetime=strftime("%Y-%m-%d %H:%M:%S", gmtime()), id_user=session["user_id"])

                # update user cash
                cash = db.execute("UPDATE 'users' SET cash = :cash WHERE id = :id_user",
                                  cash=cash[0]["cash"] - stock["price"] * num_stock, id_user=session["user_id"])

                portfolio_stocks = db.execute(
                    "SELECT * FROM 'portfolio' WHERE id_user = :id_user AND symbol = :symbol", id_user=session["user_id"], symbol=stock["symbol"])

                # insert or update portfolio
                if len(portfolio_stocks) == 0:
                    portfolio_stocks_insrt = db.execute("INSERT INTO 'portfolio' (symbol, num_stocks, id_user) VALUES (:symbol, :num_stocks, :id_user)",
                                                        symbol=stock["symbol"], num_stocks=request.form.get("shares"), id_user=session["user_id"])
                elif len(portfolio_stocks) >= 1:
                    portfolio_stocks_update = db.execute("UPDATE 'portfolio' SET num_stocks = :num_stocks WHERE id = :id_portfolio",
                                                         num_stocks=portfolio_stocks[0]["num_stocks"] + num_stock, id_portfolio=portfolio_stocks[0]["id"])
            else:
                return apology("Money not enough", 400)

            # get all stocks of current user
        stocks = db.execute("SELECT * FROM 'portfolio' WHERE id_user = :user_id", user_id=session["user_id"])

        # initialisation variable for all money in stocks
        all_stocks = 0

        # check user cash
        cash = db.execute("SELECT cash FROM 'users' WHERE id = :user_id", user_id=session["user_id"])

        num = 0

        # check current price of stock
        if len(stocks) > 0:
            for stock in stocks:
                info = lookup(stock["symbol"])
                num += 1
                stock.update({'cur_price': 0, 'cost': 0, 'number': num})
                if info != None:
                    stock["cur_price"] = info["price"]
                    stock["cost"] = stock["cur_price"] * stock["num_stocks"]
                all_stocks += stock["cost"]

            return render_template("index.html", stocks=stocks, all_stocks=all_stocks, cash=cash[0]["cash"])
    return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    transactions = db.execute("SELECT * FROM 'transaction' WHERE id_user = :user_id ORDER BY date DESC", user_id=session["user_id"])

    if len(transactions) > 0:
        return render_template("history.html", transactions=transactions)
    else:
        return render_template("history.html")


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 400)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 400)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 400)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    # route via POST
    if request.method == "POST":

        # check if stock empty
        if not request.form.get("symbol"):
            return apology("Enter stock name", 400)

        # if stock not empty
        stocks = lookup(request.form.get("symbol"))

        # if stocks api don't return any result
        if stocks == None:
            return apology("This stock is no exist", 400)

        # if stocks return result
        else:
            return render_template("quoted.html", symbol=stocks['symbol'], price=stocks['price'])

    return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    # user reach method via POST
    if request.method == "POST":

        # username was submitted
        if not request.form.get("username"):
            return apology("Missing username!", 400)
        # password was submitted
        elif not request.form.get("password"):
            return apology("Missing password!", 400)
        # copy password was submitted
        elif not request.form.get("confirmation"):
            return apology("Missing copy password!", 400)

        elif request.form.get("confirmation") != request.form.get("password"):
            return apology("Passwords not equals!", 400)

        # check username if userbase exist in database
        check = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))
        if len(check) != 1:
            insert = db.execute("INSERT INTO users (username, hash) VALUES (:username, :pass_hash)",
                                username=request.form.get("username"), pass_hash=generate_password_hash(request.form.get("password")))
        elif len(check) == 1:
            return("This username already exist!", 400)

    return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    # check user cash
    cash = db.execute("SELECT cash FROM 'users' WHERE id = :user_id", user_id=session["user_id"])

    # check if user any stocks
    stocks = db.execute("SELECT * FROM 'portfolio' WHERE id_user = :user_id",
                        user_id=session["user_id"])

    if request.method == "POST":

        # check text on input field
        if not request.form.get("symbol"):
            return apology("Missing stock!", 400)
        elif not request.form.get("symbol"):
            return apology("Missing number of stocks!", 400)
        # check is not alpha in stocks name input
        elif not request.form.get("symbol").isalpha():
            return apology("Stock name not correct!", 400)
        # check for positive integer in num_stocks input
        elif int(request.form.get("shares")) <= 0:
            return apology("Number stocks must be positive", 400)

        # if stock not empty
        stocks = lookup(request.form.get("symbol"))

        # if stocks api don't return any result
        if stocks == None:
            return apology("This stock is no exist", 400)

        # check if user have this stocks
        stock_portfolio = db.execute("SELECT * FROM 'portfolio' WHERE id_user = :user_id AND symbol = :symbol",
                                     user_id=session["user_id"], symbol=request.form.get("symbol"))

        # check if stocks enough
        if len(stock_portfolio) > 0:
            if stock_portfolio[0]["num_stocks"] >= int(request.form.get("shares")):
                # update count of stocks in portfolio
                stock_portfolio = db.execute("UPDATE 'portfolio' SET 'num_stocks' = :num_stocks WHERE id_user = :user_id AND symbol = :symbol", num_stocks=(
                    stock_portfolio[0]["num_stocks"] - int(request.form.get("shares"))), user_id=session["user_id"], symbol=request.form.get("symbol"))
                # add sell transaction to transaction table
                transaction = db.execute("INSERT INTO 'transaction' (symbol, price, num_stocks, action, date, id_user) VALUES (:symbol, :price, :num_stocks, :action, :datetime, :id_user)", symbol=request.form.get(
                    "symbol"), price=stocks["price"], num_stocks=request.form.get("shares"), action="sell", datetime=strftime("%Y-%m-%d %H:%M:%S", gmtime()), id_user=session["user_id"])
                # update cash in user table
                cash = db.execute("UPDATE 'users' SET 'cash' = :cash WHERE id = :user_id",
                                  cash=cash[0]['cash'] + (int(request.form.get("shares")) * stocks["price"]), user_id=session["user_id"])
            else:
                return apology("Num of stocks not enought", 400)
        else:
            return apology("User don't have this stocks", 400)

        # get all stocks of current user
        stocks = db.execute("SELECT * FROM 'portfolio' WHERE id_user = :user_id", user_id=session["user_id"])

        # initialisation variable for all money in stocks
        all_stocks = 0

        # check user cash
        cash = db.execute("SELECT cash FROM 'users' WHERE id = :user_id", user_id=session["user_id"])

        num = 0

        # check current price of stock
        if len(stocks) > 0:
            for stock in stocks:
                info = lookup(stock["symbol"])
                num += 1
                stock.update({'cur_price': 0, 'cost': 0, 'number': num})
                if info != None:
                    stock["cur_price"] = info["price"]
                    stock["cost"] = stock["cur_price"] * stock["num_stocks"]
                all_stocks += stock["cost"]

            return render_template("index.html", stocks=stocks, all_stocks=all_stocks, cash=cash[0]["cash"])
    return render_template("sell.html", stocks=stocks)


def errorhandler(e):
    """Handle error"""
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
