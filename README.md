# Smart Reminder Application

A console-based (with partial GUI using Windows API) C++ application that allows users to create, manage, and receive notifications for personal reminders. The project demonstrates key object-oriented programming concepts including encapsulation, inheritance, polymorphism, and multithreading.

## 📌 Features

- **User Management**
  - Registration with password authentication
  - Per-user data files

- **Reminder Management**
  - Add, view, modify, delete, and search reminders
  - Support for one-time and recurring reminders (daily, weekly, monthly, yearly)

- **Real-Time Notifications**
  - Popup alerts using Windows API (`MessageBoxA`)
  - Audio alerts using `Beep`
  - Snooze feature (5-minute delay)

- **Persistence**
  - File-based storage using custom `.txt` format
  - Data retained across sessions

- **Multithreading**
  - Background thread for real-time reminder checks every second

## 🛠 Technical Details

- **Language & Standard**: C++17
- **Editor/Compiler**: Visual Studio Code with MinGW GCC 11+
- **Platform**: Windows 10+
- **Key Libraries**: `<thread>`, `<chrono>`, `<fstream>`, Windows API

## 🧱 Architecture

- `Reminder` (base class) – Stores common task attributes
- `RecurringReminder` (derived class) – Handles recurring task logic
- `FileManager` – Manages file I/O for users and reminders
- `ReminderManager` – Provides all reminder CRUD operations
- `NotificationManager` – Runs a separate thread to monitor and trigger due reminders
- `UserManager` – Manages user registration and login

## ✅ Testing

The application was rigorously tested through manual and functional testing:

- Verified functionality of all CRUD operations
- Ensured data integrity across multiple sessions using various file states
- Tested edge cases like long strings, special characters, and invalid input
- Verified OOP design by testing base and derived classes via polymorphic calls
- Ensured notification accuracy and responsiveness through timed testing

## 👥 Team

- Harsh (24K0558) – Reminder classes, file management, threading
- Khubaib ur Rehman (24K0558) – NotificationManager, Windows API, testing
- Aaraiz (24K0620) – CRUD operations, search, testing

## 📅 Submission Date

April 20, 2025

## 📈 Future Improvements

- Cross-platform GUI using Qt or wxWidgets
- Support for advanced recurrence rules (e.g., "first Monday of the month")
- Database integration for better data management and multi-user concurrency

## 📜 License

This project is for educational purposes and is not licensed for commercial distribution.
