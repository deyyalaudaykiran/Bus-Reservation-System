# Bus-Reservation-System
A complete Console-Based Bus Reservation System developed in C Programming with features like seat booking, passenger management, file handling, and automated email ticket confirmation using libcurl SMTP integration.
This project simulates a real-world bus ticket booking platform where users can:

View available buses
Reserve seats
Manage passenger details
Visualize seat availability
Prevent invalid adjacent seat bookings
Receive booking confirmation emails automatically
✨ Key Features
🎟️ Smart Ticket Booking
Reserve seats with real-time availability checking
Prevents double booking
Gender-based adjacent seat restriction system
🪑 Interactive Seat Layout
Visual seat mapping in terminal
Color-coded seats:
🟢 Male
🟣 Female
⚪ Available
📧 Automated Email Notifications
Sends professional booking confirmation emails
Uses Gmail SMTP + libcurl
Includes:
Journey details
Passenger information
Seat number
Fare
Booking timestamp
💾 File Handling
Persistent data storage using text files
Stores:
Bus information
Booking records
Seat status
🚌 Bus Management
Add new bus services dynamically
Store:
Bus type
Journey date
Departure time
Fare details
📋 Booking Records
View all reservations
Displays passenger details and booked seats
🛠️ Technologies Used
C Programming
Data Structures (Linked List)
File Handling
libcurl SMTP API
Terminal ANSI Color Formatting
📂 Project Modules
Bus Management
Seat Reservation
Booking Management
Email Notification System
File Storage System
🔒 Special Logic Implemented

✔️ Gender-based adjacent seat validation
✔️ Dynamic linked list booking storage
✔️ Persistent seat synchronization
✔️ SMTP secure email integration
✔️ Structured modular programming

🚀 Future Enhancements
Ticket Cancellation Feature
Admin Login System
Payment Gateway Integration
Search & Filter Buses
GUI Version
Database Integration (MySQL)
📸 Sample Functionalities
Add Bus Services
Book Tickets
Display Seat Layout
Send Email Confirmation
View Passenger Records
👨‍💻 Author

⚙️ Setup & Installation Guide
📥 Clone the Repository

git clone https://github.com/deyyalaudaykiran/bus-reservation-system.git
cd bus-reservation-system
🛠️ Required Libraries

This project uses:

libcurl → for sending email notifications using SMTP
Standard C libraries:
stdio.h
stdlib.h
string.h
time.h
📦 Install libcurl
🔹 Ubuntu / Debian
sudo apt update
sudo apt install libcurl4-openssl-dev
🔹 Fedora
sudo dnf install libcurl-devel
🔹 Arch Linux
sudo pacman -S curl


▶️ Compile the Project
Linux / macOS
gcc bus_reservation.c -o bus_reservation -lcurl
Windows (MinGW)
gcc bus_reservation.c -o bus_reservation.exe -lcurl
▶️ Run the Application
Linux / macOS
./bus_reservation
Windows
bus_reservation.exe
📁 Generated Files

The system automatically creates:

File Name	Purpose
buses.txt	Stores bus details
bookings.txt	Stores booking records
📧 Gmail SMTP Setup

To enable email notifications:

Enable 2-Step Verification in your Gmail account
Generate an App Password
Replace these values in the code:
#define FROM_EMAIL "your-email@gmail.com"
#define APP_PASSWORD "your-app-password"
✅ Features Available
View Bus Services
Add New Buses
Book Tickets
Seat Visualization
Email Confirmation
View Bookings
🚨 Important Notes
Internet connection is required for email notifications
Keep your App Password secure
Do NOT upload real credentials to GitHub

Recommended:

Use environment variables or config files for credentials
🧹 Clean Build

To remove compiled files:

rm bus_reservation

Developed as a mini-project to demonstrate:

Real-world problem solving
File handling in C
Data structure implementation
API integration in C

⭐ If you like this project, don't forget to star the repository!
