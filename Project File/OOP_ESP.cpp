#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <chrono>
#include <vector>
#include <windows.h>
#include <thread>
#include <iomanip>
#include <ctime>
#include <atomic>

using namespace std;
using namespace std::chrono;


class Reminder {
protected:
    string title;
    string description;
    string date; // format: YYYY-MM-DD
    string time; // format: HH:MM:SS
    string recurrence;
    int status;
public:
    Reminder(string title, string description, string date, string time, string recurrence, int status){
        this->title = title;
        this->description = description;
        this->date = date;
        this->time = time;
        this->recurrence = recurrence;
        this->status = status;
    }

    virtual void display() {
        cout << left << setw(20) << title
             << setw(15) << date
             << setw(15) << time
             << setw(100) << description
             << setw(15) << (status == 1 ? "Completed" : (status == 0 ? "Incomplete" : "Reschedule"))
             << setw(15) << recurrence << endl;
    }

    string getTitle(){ return title; }
    string getRecurrence(){ return recurrence; }
    string getDescription(){ return description; }
    string getDate(){ return date; }
    string getTime(){ return time; }
    int getStatus(){ return status; }

    void setTitle(string title){ this->title = title; }
    void setDescription(string description){ this->description = description; }
    void setDate(string date){ this->date = date; }
    void setRecurrence(string recurrence){ this->recurrence = recurrence; }
    void setTime(string time){ this->time = time; }
    void setStatus(int status){ this->status = status; }

    virtual ~Reminder() { }
};

class RecurringReminder : public Reminder {
public:
    RecurringReminder(string title, string description, string date, string time, string recurrence, int status)
        : Reminder(title, description, date, time, recurrence, status) { }

    void display() override {
        cout << left << setw(20) << (title + " (Recurring)")
             << setw(15) << date
             << setw(15) << time
             << setw(100) << description
             << setw(15) << (status == 1 ? "Completed" : (status == 0 ? "Incomplete" : "Reschedule"))
             << setw(15) << recurrence << endl;
    }
};

class FileManager {
public:

    static void writeToFile(string fileName, string title, string description, string date, string time, string recurrence, int status){
        ofstream file(fileName, ios::app);
        if(file.is_open()){
            file << title << "&" << description << "&" << date << "&" << time << "&" << status << "&" << recurrence << "\n";
            file.close();
        } else {
            cout << "An error occurred while saving the reminder." << endl;
        }
    }

    static void readFromFile(vector<Reminder*>& vec, string fileName) {
        ifstream file(fileName, ios::in);
        if (file.is_open()) {
            string line;
            getline(file, line);

            while (getline(file, line)) {
                string title, description, date, time, recurrence;
                string tempStatus = "";
                int status;
                int count = 1;
                for (int i = 0; i < line.length(); i++) {
                    if (line[i] == '&') {
                        count++;
                    } else {
                        if (count == 1) {
                            title += line[i];
                        } else if (count == 2) {
                            description += line[i];
                        } else if (count == 3) {
                            date += line[i];
                        } else if (count == 4) {
                            time += line[i];
                        } else if (count == 5) {
                            tempStatus += line[i];
                        } else if (count == 6) {
                            recurrence += line[i];
                        }
                    }
                }
                status = stoi(tempStatus);
                if(recurrence != "none"){
                    vec.push_back(new RecurringReminder(title, description, date, time, recurrence, status));
                } else {
                    vec.push_back(new Reminder(title, description, date, time, recurrence, status));
                }
            }
            file.close();
            cout << "Reminders loaded successfully! Total reminders: " << vec.size() << endl;
        } else {
            cout << "Error: Could not open file for reading." << endl;
        }
    }

    static void finalSaveToFile(string username, string password, vector<Reminder*>& vec){
        ofstream file(username + ".txt", ios::out);
        if(file.is_open()){
            file << "password:" << password << "\n";
            for (int i = 0; i < vec.size(); i++){
                file << vec[i]->getTitle() << "&" << vec[i]->getDescription() << "&"
                     << vec[i]->getDate() << "&" << vec[i]->getTime() << "&"
                     << vec[i]->getStatus() << "&" << vec[i]->getRecurrence() << "\n";
            }
            file.close();
        } else {
            cout << "An error occurred while saving the reminder." << endl;
        }
    }

    static int checkFile(string fileName){
        ifstream file(fileName);
        if(file.is_open()){
            return 1;
        } else {
            return 0;
        }
    }
};

class NotificationManager {
public:
    // Sound alert
    static void sound(){
        Beep(750, 500);
    }

    static void PopupNotification(Reminder& obj) {
        string alertMessage = "Title: " + obj.getTitle() + "\n" +
                              "Description: " + obj.getDescription() + "\n" +
                              "Date: " + obj.getDate() + "\n" +
                              "Time: " + obj.getTime();

        sound();

        int msgboxID = MessageBoxA(
            NULL,
            alertMessage.c_str(),
            "Reminder Notification",
            MB_YESNO | MB_ICONASTERISK
        );

        if (msgboxID == IDNO) {
            sound();
            int snoozeID = MessageBoxA(
                NULL,
                "Do you want to snooze this Task for 5 minutes?",
                "TASK RESCHEDULING",
                MB_YESNO
            );

            if (snoozeID == IDYES) {
                auto now = system_clock::now();
                time_t now_time = system_clock::to_time_t(now);
                tm now_tm = *localtime(&now_time);

                now_tm.tm_min += 5;
                if (now_tm.tm_min >= 60) {
                    now_tm.tm_min -= 60;
                    now_tm.tm_hour += 1;
                    if (now_tm.tm_hour >= 24) {
                        now_tm.tm_hour = 0;
                        now_tm.tm_mday += 1;
                    }
                }

                char newTime[9];
                strftime(newTime, sizeof(newTime), "%H:%M:%S", &now_tm);
                obj.setTime(newTime);
                cout << "\nTask snoozed to: " << newTime << endl;
                cout<<endl;
                cout << "1. Add Reminder\n";
                cout << "2. Modify Reminder\n";
                cout << "3. Delete Reminder\n";
                cout << "4. View Reminders\n";
                cout << "5. Search Reminder\n";
                cout << "6. Logout\n";
                cout << endl;
                cout << "Enter your choice: ";
            } else {
                cout<<"\nTask needs rescheduleing\n";
                obj.setStatus(2);
                cout<<endl;
                cout << "1. Add Reminder\n";
                cout << "2. Modify Reminder\n";
                cout << "3. Delete Reminder\n";
                cout << "4. View Reminders\n";
                cout << "5. Search Reminder\n";
                cout << "6. Logout\n";
                cout << endl;
                cout << "Enter your choice: ";
            }
        } else if (msgboxID == IDYES) {
            obj.setStatus(1);
            cout << "\nTask marked as completed." << endl;
            cout<<endl;
            cout << "1. Add Reminder\n";
            cout << "2. Modify Reminder\n";
            cout << "3. Delete Reminder\n";
            cout << "4. View Reminders\n";
            cout << "5. Search Reminder\n";
            cout << "6. Logout\n";
            cout << endl;
            cout << "Enter your choice: ";
            string recurrence = obj.getRecurrence();
            if (recurrence != "none") {
                auto now = system_clock::now();
                time_t now_time = system_clock::to_time_t(now);
                tm now_tm = *localtime(&now_time);

                if (recurrence == "daily") {
                    now_tm.tm_mday += 1;
                } else if (recurrence == "weekly") {
                    now_tm.tm_mday += 7;
                } else if (recurrence == "monthly") {
                    now_tm.tm_mon += 1;
                } else if (recurrence == "yearly") {
                    now_tm.tm_year += 1;
                }

                time_t new_time = mktime(&now_tm);
                tm* new_tm = localtime(&new_time);

                char newDate[11];
                strftime(newDate, sizeof(newDate), "%Y-%m-%d", new_tm);
                obj.setDate(newDate);
                cout << "Task rescheduled to: " << newDate << endl;
            }
        }
    }

    static void checkAndNotify(vector<Reminder*>& reminders, atomic<bool>& running) {
        while (running) {
            auto now = system_clock::now();
            time_t now_time = system_clock::to_time_t(now);

            for (auto& reminder : reminders) {
                if (reminder->getStatus() == 0) { 
                    tm reminder_tm = {};
                    sscanf(reminder->getDate().c_str(), "%d-%d-%d", 
                           &reminder_tm.tm_year, &reminder_tm.tm_mon, &reminder_tm.tm_mday);
                    sscanf(reminder->getTime().c_str(), "%d:%d:%d", 
                           &reminder_tm.tm_hour, &reminder_tm.tm_min, &reminder_tm.tm_sec);
                    reminder_tm.tm_year -= 1900;
                    reminder_tm.tm_mon -= 1;
                    time_t reminder_time = mktime(&reminder_tm);

                    if (now_time >= reminder_time) {
                        PopupNotification(*reminder);
                    }
                }
            }
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
};

class ReminderManager {
private:
    vector<Reminder*> reminders;
public:
    void addReminder() {
        string title, description, recurrence, date, time;
        cout << "Enter Title: ";
        cin.ignore();
        getline(cin, title);
        cout << "Enter Description: ";
        getline(cin, description);
        cout << "Enter Date (YYYY-MM-DD): ";
        cin >> date;
        cout << "Enter Time (HH:MM:SS): ";
        cin >> time;
        cout << "Want the task to reoccur (daily/monthly/yearly/none): ";
        cin.ignore();
        getline(cin, recurrence);

        if(recurrence != "none") {
            reminders.push_back(new RecurringReminder(title, description, date, time, recurrence, 0));
        } else {
            reminders.push_back(new Reminder(title, description, date, time, recurrence, 0));
        }
        cout << "Reminder added successfully!" << endl;
    }

    void modifyReminder(string title, string change, string data) {
        bool found = false;
        for (int i = 0; i < reminders.size(); i++) {
            if (reminders[i]->getTitle() == title) {
                if (change == "title") {
                    reminders[i]->setTitle(data);
                } else if (change == "description") {
                    reminders[i]->setDescription(data);
                } else if (change == "date") {
                    reminders[i]->setDate(data);
                    reminders[i]->setStatus(0);
                } else if (change == "time") {
                    reminders[i]->setTime(data);
                    reminders[i]->setStatus(0);
                } else if (change == "recurrance") {
                    reminders[i]->setRecurrence(data);
                }
                found = true;
                break;
            }
        }
        if (found) {
            cout << "Reminder modified successfully" << endl;
        } else {
            cout << "Reminder not found" << endl;
        }
    }

    void deleteReminder(string title) {
        bool found = false;
        for (int i = 0; i < reminders.size(); i++) {
            if (reminders[i]->getTitle() == title) {
                delete reminders[i];
                reminders.erase(reminders.begin() + i);
                found = true;
                break;
            }
        }
        if (found) {
            cout << "Reminder deleted successfully" << endl;
        } else {
            cout << "Reminder not found" << endl;
        }
    }

    void viewReminders() {
        if (reminders.empty()) {
            cout << "No reminders found." << endl;
            return;
        }
        cout << left << setw(20) << "Title" 
             << setw(15) << "Date" 
             << setw(15) << "Time" 
             << setw(100) << "Description" 
             << setw(15) << "Status"
             << setw(15) << "Recurrence" << endl;
        cout << string(185, '-') << endl;
        for (int i = 0; i < reminders.size(); i++) {
            reminders[i]->display();
        }
        cout << string(185, '-') << endl;
    }

    int searchReminder() {
        string search;
        cout << "Enter the title of the reminder you want to search for: ";
        cin.ignore();
        getline(cin, search);
        for (int i = 0; i < reminders.size(); i++) {
            if (reminders[i]->getTitle() == search){
                return i;
            }
        }
        cout << "No reminder found matching the keyword.";
        return -1;
    }

    vector<Reminder*>& getReminders() {
        return reminders;
    }

    void clearReminders() {
        for (int i = 0; i < reminders.size(); i++) {
            delete reminders[i];
        }
        reminders.clear();
    }

    ~ReminderManager() {
        clearReminders();
    }
};

class UserManager {
public:

    static void userRegister(string username, string password, vector<Reminder*>& reminders) {
        string fileName = username + ".txt";
        int check = FileManager::checkFile(fileName);
        if (check == 1) {
            cout << "Username already exists" << endl;
            return;
        } else {
            ofstream file(fileName, ios::app);
            if(file.is_open()){
                file << "password:" << password << "\n";
                file.close();
                for (int i = 0; i < reminders.size(); i++){
                    FileManager::writeToFile(fileName, reminders[i]->getTitle(), reminders[i]->getDescription(),
                                                reminders[i]->getDate(), reminders[i]->getTime(),
                                                reminders[i]->getRecurrence(), reminders[i]->getStatus());
                }
                cout << "User registered successfully" << endl;
            } else {
                cout << "An error occurred while registering the user." << endl;
            }
        }
    }

    static string login(string username, string password, vector<Reminder*>& reminders) {
        string fileName = username + ".txt";
        ifstream file(fileName, ios::in);
        if (file.is_open()) {
            string line;
            getline(file, line);
            string currentPassword = line.substr(9);
            if (password == currentPassword) {
                file.close();
                FileManager::readFromFile(reminders, fileName);
                return "success";
            } else {
                return "password error";
            }
        } else {
            return "username error";
        }
    }
};

int main(){
    int choice;
    string username, password;
    ReminderManager reminderManager;
    atomic<bool> notificationRunning(false);
    thread notificationThread;
    bool flag = false;

    while (true) {
        cout << "\n==== Smart Reminder App ====\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            vector<Reminder*> tempReminders;
            UserManager::userRegister(username, password, tempReminders);
        } else if (choice == 2) {
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            string loginResult = UserManager::login(username, password, reminderManager.getReminders());
            if (loginResult == "password error") {
                cout << "Invalid password!" << endl;
            } else if (loginResult == "username error") {
                cout << "Username not found" << endl;
            } else {
                flag = true;
            }

            if (flag == true) {
                notificationRunning = true;
                notificationThread = thread(NotificationManager::checkAndNotify, ref(reminderManager.getReminders()), ref(notificationRunning));
                
                int option;
                while (true) {
                    cout << "\n==== Welcome, " << username << " ====\n";
                    cout << "1. Add Reminder\n";
                    cout << "2. Modify Reminder\n";
                    cout << "3. Delete Reminder\n";
                    cout << "4. View Reminders\n";
                    cout << "5. Search Reminder\n";
                    cout << "6. Logout\n";
                    cout << endl;
                    cout << "Enter your choice: ";
                    cin >> option;

                    if (option == 1) {
                        reminderManager.addReminder();
                    } else if (option == 2) {
                        string title, change, data;
                        cout << "Enter the title of the reminder to modify: ";
                        cin.ignore();
                        getline(cin, title);
                        cout << "Enter the field to change (title/description/date/time/recurrance): ";
                        getline(cin, change);
                        cout << "Enter the new data: ";
                        getline(cin, data);
                        reminderManager.modifyReminder(title, change, data);
                    } else if (option == 3) {
                        string title;
                        cout << "Enter the title of the reminder to delete: ";
                        cin.ignore();
                        getline(cin, title);
                        reminderManager.deleteReminder(title);
                    } else if (option == 4) {
                        cout << endl;
                        reminderManager.viewReminders();
                    } else if (option == 5) {
                        int retval = reminderManager.searchReminder();
                        if(retval != -1){
                            cout << endl;
                            vector<Reminder*>& rems = reminderManager.getReminders();
                            cout << left << setw(20) << "Title" 
                                 << setw(15) << "Date" 
                                 << setw(15) << "Time" 
                                 << setw(115) << "Description" 
                                 << setw(15) << "Status" << endl;
                            cout << string(185, '-') << endl;
                            rems[retval]->display();
                            cout << string(185, '-') << endl;
                        } else {
                            cout << "Invalid choice! Please try again." << endl;
                        }
                    } else if (option == 6) {
                        cout << "Logging out..." << endl;
                        notificationRunning = false;
                        if (notificationThread.joinable()) {
                            notificationThread.join();
                        }
                        FileManager::finalSaveToFile(username, password, reminderManager.getReminders());
                        reminderManager.clearReminders();
                        break;
                    } else {
                        cout << "Invalid choice! Please try again." << endl;
                    }
                }
            }
        } else if (choice == 3) {
            cout << "Exiting application..." << endl;
            break;
        } else {
            cout << "Invalid choice! Please try again." << endl;
        }
    }
    return 0;
}
