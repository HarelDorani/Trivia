import requests
import sqlite3
import html
import random

# Connect to database
conn = sqlite3.connect('triviaDB.sqlite')
c = conn.cursor()

# Create table QUESTIONS
c.execute('''
CREATE TABLE IF NOT EXISTS QUESTIONS (
    ID INTEGER PRIMARY KEY AUTOINCREMENT,
    QUESTION TEXT NOT NULL,
    ANSWER1 TEXT NOT NULL,
    ANSWER2 TEXT NOT NULL,
    ANSWER3 TEXT NOT NULL,
    ANSWER4 TEXT NOT NULL,
    CORRECT_ANSWER INTEGER NOT NULL CHECK (CORRECT_ANSWER BETWEEN 1 AND 4)
)
''')

# Fetch 15 questions
response = requests.get("https://opentdb.com/api.php?amount=15&type=multiple")
data = response.json()

if data["response_code"] == 0:
    for item in data["results"]:
        question = html.unescape(item["question"])
        correct = html.unescape(item["correct_answer"])
        incorrects = [html.unescape(ans) for ans in item["incorrect_answers"]]

        all_answers = incorrects + [correct]
        random.shuffle(all_answers)

        correct_index = all_answers.index(correct) + 1  # 1-based index

        # Insert into QUESTIONS table
        c.execute('''
        INSERT INTO QUESTIONS (QUESTION, ANSWER1, ANSWER2, ANSWER3, ANSWER4, CORRECT_ANSWER)
        VALUES (?, ?, ?, ?, ?, ?)
        ''', (question, *all_answers, correct_index))

    conn.commit()
    print("✅ 15 questions inserted successfully into QUESTIONS table.")
else:
    print("❌ Failed to fetch questions. API response_code:", data["response_code"])

conn.close()
