# Hospital Management System

A management system for a hospital named Medicore is built is with using beginner level concepts of Object Oriented Programming in C++. MediCore Hospital Management System is a console-based Object-Oriented Programming (OOP) project in C++ designed to simulate real-world hospital operations. The system manages patients, doctors, appointments, prescriptions, billing, and records, ensuring persistent storage using file handling.

This project strictly follows OOP principles such as encapsulation, inheritance, polymorphism, abstraction, and includes operator overloading, templates, and exception handling.

🚀 Features

👤 User Roles

The system supports three types of users:

* Patient
* Doctor
* Admin


Each role has its own menu and functionalities.

🧑‍⚕️ Patient Functionalities


* Book appointments with doctors
* Cancel appointments (with refund)
* View appointment history (sorted by date)
* View medical records (prescriptions)
* View and pay bills
* Top-up account balance



👨‍⚕️ Doctor Functionalities



* View today's appointments
* Mark appointments as completed or no-show
* Write prescriptions
* View patient medical history (restricted access)
* 

🛠️ Admin Functionalities



* Add/remove doctors
* View all patients and doctors
* View all appointments
* Track unpaid bills
* Discharge patients (with record archiving)
* View security logs
* Generate daily reports
* 

🧱 Project Structure



Each class is implemented in separate .h and .cpp files.



🔹 Core Classes



Person (Abstract Base Class)

Patient

Doctor

Admin

Appointment

Bill

Prescription



🔹 Utility Classes



Storage<T> (Template class using static array)

FileHandler (Handles all file operations)

Validator (Handles input validation)



🔹 Exception Classes



* HospitalException
* FileNotFoundException
* InsufficientFundsException
* InvalidInputException
* SlotUnavailableException
* 

⚙️ Key Concepts Implemented



* Object-Oriented Design
* Operator Overloading
* == for comparisons
* << for output
* += and -= for balance updates
* Templates (Storage Class)
* Custom Exception Handling
* Dynamic Memory Management (No memory leaks)
* Manual Sorting Algorithms (No STL sort)
* File Handling (Persistent Data Storage)

📂 File Storage Format



All data is stored in .txt files using comma-separated values (CSV format).



* Files Used:
* patients.txt
* doctors.txt
* admin.txt
* appointments.txt
* prescriptions.txt
* bills.txt
* security\_log.txt
* discharged.txt
* 

🖥️ How to Compile and Run

🧰 Requirements

C++ Compiler (e.g., g++ or Visual Studio 2022)

No external libraries required

