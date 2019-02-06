import os
import re
import sys
from flask import Flask, jsonify, render_template, request

from cs50 import SQL
from helpers import lookup

# Configure application
app = Flask(__name__)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///mashup.db")


# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
def index():
    """Render map"""
    return render_template("index.html")


@app.route("/articles")
def articles():
    """Look up articles for geo"""

    # Check if parametr geo exist
    if not request.args.get("geo"):
        raise RuntimeError("missing geo")

    # Get articles from API
    articles = lookup(request.args.get("geo"))

    return jsonify(articles)


@app.route("/search")
def search():
    """Search for places that match query"""
    # check if parametr geo exist
    if not request.args.get("q"):
        raise RuntimeError("missing query")

    # Get first world
    query = request.args.get("q").split(" ")

    query_len = len(query)

    for i in range(query_len):
        if query[i].endswith(","):
            query[i] = query[i][:-1]
        query[i] += "%"

    # Query is digit search in
    if query[0][:-1].isdigit():
        data = db.execute("SELECT * FROM places WHERE postal_code LIKE :query0", query0=query[0])

    # Query is alphabetic search for one word
    elif query_len == 1 and query[0][:-1].isalpha():
        data = db.execute("SELECT * FROM places WHERE place_name LIKE :query0", query0=query[0])

    # Query search for more then one word
    elif query_len == 2 and query[0][:-1].isalpha() and query[1][:-1].isalpha():
        data = db.execute("""SELECT * FROM places
                             WHERE (place_name LIKE :query0 AND admin_name1 LIKE :query1)
                             OR (place_name LIKE :query0 AND admin_code1 LIKE :query1)""",
                          query0=query[0], query1=query[1])

    return jsonify(data)


@app.route("/update")
def update():
    """Find up to 10 places within view"""

    # Ensure parameters are present
    if not request.args.get("sw"):
        raise RuntimeError("missing sw")
    if not request.args.get("ne"):
        raise RuntimeError("missing ne")

    # Ensure parameters are in lat,lng format
    if not re.search("^-?\d+(?:\.\d+)?,-?\d+(?:\.\d+)?$", request.args.get("sw")):
        raise RuntimeError("invalid sw")
    if not re.search("^-?\d+(?:\.\d+)?,-?\d+(?:\.\d+)?$", request.args.get("ne")):
        raise RuntimeError("invalid ne")

    # Explode southwest corner into two variables
    sw_lat, sw_lng = map(float, request.args.get("sw").split(","))

    # Explode northeast corner into two variables
    ne_lat, ne_lng = map(float, request.args.get("ne").split(","))

    # Find 10 cities within view, pseudorandomly chosen if more within view
    if sw_lng <= ne_lng:

        # Doesn't cross the antimeridian
        rows = db.execute("""SELECT * FROM places
                          WHERE :sw_lat <= latitude AND latitude <= :ne_lat AND (:sw_lng <= longitude AND longitude <= :ne_lng)
                          GROUP BY country_code, place_name, admin_code1
                          ORDER BY RANDOM()
                          LIMIT 10""",
                          sw_lat=sw_lat, ne_lat=ne_lat, sw_lng=sw_lng, ne_lng=ne_lng)

    else:

        # Crosses the antimeridian
        rows = db.execute("""SELECT * FROM places
                          WHERE :sw_lat <= latitude AND latitude <= :ne_lat AND (:sw_lng <= longitude OR longitude <= :ne_lng)
                          GROUP BY country_code, place_name, admin_code1
                          ORDER BY RANDOM()
                          LIMIT 10""",
                          sw_lat=sw_lat, ne_lat=ne_lat, sw_lng=sw_lng, ne_lng=ne_lng)

    # Output places as JSON
    return jsonify(rows)
