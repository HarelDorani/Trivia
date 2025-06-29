# Trivia

Magshimim's second year final project, built together with a classmate over three months.

A trivia game you can play with your friends in private rooms. The system is built with a C++ backend server and a C# WPF GUI client.

---

## 🎮 Features

- Login / Signup system
- Create and join rooms
- Wait in lobby with player list
- Real-time multiplayer trivia game
- Timer for each question
- Score and game results screen
- View personal statistics and high scores
- Background images and styled UI

---

## 🏗️ Tech Stack

- **Server:** C++17
- **Client GUI:** C# (WPF)
- **Data Storage:** SQLite
- **Communication:** TCP Sockets
- **Serialization:** JSON 

---

## 🚀 How to Run

### Server (C++)
1. Open with Visual Studio 2022 (or later)
2. Build the solution
3. Run `main.cpp` in the `trivia` project
4. Server will listen on port `2000`

### Client (C# WPF)
1. Open the `clientApp` project in Visual Studio
2. Set as startup project
3. Build and run
4. Enter the server IP and start playing


