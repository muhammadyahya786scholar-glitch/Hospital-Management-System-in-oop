#define _CRT_SECURE_NO_WARNINGS   // suppress MSVC localtime/strftime warnings

/* ============================================================
 *  MediCore Hospital Management System
 *  Course  : Object Oriented Programming (OOP) — Spring 2026
 *  Section : BCS-2G
 * ============================================================
 *
 *  ALL in one .cpp file as requested.
 *  Constraints honoured:
 *    - No std::string, no std::vector
 *    - No strcmp / strtok / strstr  (manual char-array logic used)
 *    - No static arrays for data  (Storage<T> uses dynamic array)
 *    - No global variables
 *    - No goto
 *    - Dynamic allocation throughout; memory freed after use
 *    - All data read from / written to .txt files
 *    - Operator overloading as required
 *    - Custom exceptions
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cctype>   // tolower — allowed (it's a single-char helper, not a string fn)

#ifdef USE_SFML_UI
// SFML UI (optional). Your backend + file handling remains unchanged.
#include <SFML/Graphics.hpp>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#if defined(_MSC_VER)
#pragma message("MediCore: USE_SFML_UI is enabled (SFML UI will be compiled).")
#endif
#endif

using namespace std;

// ============================================================
//  SECTION 1 : Utility / helper free functions
// ============================================================

/* myStrlen — returns length of a c-string */
int myStrlen(const char* s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

/* myStrcpy — copies src into dst */
void myStrcpy(char* dst, const char* src) {
    int i = 0;
    while (src[i]) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

/* myStrncpy — copies at most n chars */
void myStrncpy(char* dst, const char* src, int n) {
    int i = 0;
    while (i < n - 1 && src[i]) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

/* myStrcmp — returns 0 if equal */
int myStrcmp(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i] && a[i] == b[i]) i++;
    return (unsigned char)a[i] - (unsigned char)b[i];
}

/* myStrcmpi — case-insensitive compare */
int myStrcmpi(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i] && tolower(a[i]) == tolower(b[i])) i++;
    return tolower((unsigned char)a[i]) - tolower((unsigned char)b[i]);
}

/* myStrcat — appends src to dst */
void myStrcat(char* dst, const char* src) {
    int d = myStrlen(dst), s = 0;
    while (src[s]) dst[d++] = src[s++];
    dst[d] = '\0';
}

/* myAtoi — converts c-string to int */
int myAtoi(const char* s) {
    int i = 0, sign = 1, result = 0;
    if (s[i] == '-') { sign = -1; i++; }
    while (s[i] >= '0' && s[i] <= '9') result = result * 10 + (s[i++] - '0');
    return sign * result;
}

/* myAtof — converts c-string to float */
float myAtof(const char* s) {
    float result = 0, frac = 0;
    int i = 0;
    bool neg = false, afterDot = false;
    float divisor = 10.0f;
    if (s[i] == '-') { neg = true; i++; }
    while (s[i]) {
        if (s[i] == '.') { afterDot = true; i++; continue; }
        if (s[i] < '0' || s[i] > '9') break;
        if (!afterDot) result = result * 10 + (s[i] - '0');
        else           { frac += (s[i] - '0') / divisor; divisor *= 10; }
        i++;
    }
    result += frac;
    return neg ? -result : result;
}

/* itoa-style — converts int to c-string */
void myItoa(int n, char* buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    char tmp[20]; int i = 0;
    bool neg = false;
    if (n < 0) { neg = true; n = -n; }
    while (n > 0) { tmp[i++] = '0' + n % 10; n /= 10; }
    if (neg) tmp[i++] = '-';
    for (int j = 0; j < i; j++) buf[j] = tmp[i - 1 - j];
    buf[i] = '\0';
}

/* float to c-string with 2 decimal places */
void myFtoa(float f, char* buf) {
    bool neg = f < 0;
    if (neg) f = -f;
    int whole = (int)f;
    int frac  = (int)((f - whole) * 100 + 0.5f);
    char tmp[30]; int pos = 0;
    if (neg) tmp[pos++] = '-';
    char ibuf[20]; myItoa(whole, ibuf);
    for (int i = 0; ibuf[i]; i++) tmp[pos++] = ibuf[i];
    tmp[pos++] = '.';
    if (frac < 10) tmp[pos++] = '0';
    char fbuf[10]; myItoa(frac, fbuf);
    for (int i = 0; fbuf[i]; i++) tmp[pos++] = fbuf[i];
    tmp[pos] = '\0';
    myStrcpy(buf, tmp);
}

/* Read one token from a comma-separated line.
   idx = which field (0-based). Stores result in out[]. */
void getField(const char* line, int idx, char* out, int outLen) {
    int field = 0, i = 0, o = 0;
    out[0] = '\0';
    while (line[i] && o < outLen - 1) {
        if (line[i] == ',') { field++; i++; continue; }
        if (field == idx)   { out[o++] = line[i]; }
        else if (field > idx) break;
        i++;
    }
    out[o] = '\0';
}

/* Count commas+1 = number of fields in a line */
int countFields(const char* line) {
    int c = 1;
    for (int i = 0; line[i]; i++) if (line[i] == ',') c++;
    return c;
}

/* Get today's date as DD-MM-YYYY */
void getTodayDate(char* buf) {
    time_t t = time(nullptr);
    struct tm* tm_info = localtime(&t);
    strftime(buf, 11, "%d-%m-%Y", tm_info);
}

/* Get current timestamp as string */
void getTimestamp(char* buf) {
    time_t t = time(nullptr);
    struct tm* tm_info = localtime(&t);
    strftime(buf, 25, "%d-%m-%Y %H:%M:%S", tm_info);
}

/* Convert DD-MM-YYYY to time_t (approximate) */
time_t dateToTimeT(const char* date) {
    char d[3], m[3], y[5];
    myStrncpy(d, date,     3);
    myStrncpy(m, date + 3, 3);
    myStrncpy(y, date + 6, 5);
    struct tm t = {};
    t.tm_mday = myAtoi(d);
    t.tm_mon  = myAtoi(m) - 1;
    t.tm_year = myAtoi(y) - 1900;
    return mktime(&t);
}

/* Days between two DD-MM-YYYY dates (positive = date2 is later) */
int daysBetween(const char* date1, const char* date2) {
    double diff = difftime(dateToTimeT(date2), dateToTimeT(date1));
    return (int)(diff / 86400.0);
}

/* Read a line safely from cin, max n-1 chars */
void readLine(char* buf, int n) {
    cin.getline(buf, n);
    if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); }
}

/* Safe cin int read */
int readInt() {
    char buf[50]; readLine(buf, 50);
    return myAtoi(buf);
}

/* Safe cin float read */
float readFloat() {
    char buf[50]; readLine(buf, 50);
    return myAtof(buf);
}

// ============================================================
//  SECTION 2 : Custom Exceptions
// ============================================================

class HospitalException {
protected:
    char message[200];
public:
    HospitalException(const char* msg) { myStrncpy(message, msg, 200); }
    virtual const char* what() const { return message; }
    virtual ~HospitalException() {}
};

class FileNotFoundException : public HospitalException {
public:
    FileNotFoundException(const char* msg) : HospitalException(msg) {}
};

class InsufficientFundsException : public HospitalException {
public:
    InsufficientFundsException(const char* msg) : HospitalException(msg) {}
};

class InvalidInputException : public HospitalException {
public:
    InvalidInputException(const char* msg) : HospitalException(msg) {}
};

class SlotUnavailableException : public HospitalException {
public:
    SlotUnavailableException(const char* msg) : HospitalException(msg) {}
};

// ============================================================
//  SECTION 3 : Validator
// ============================================================

class Validator {
public:
    /* Check if string is all digits */
    static bool isAllDigits(const char* s) {
        for (int i = 0; s[i]; i++)
            if (s[i] < '0' || s[i] > '9') return false;
        return myStrlen(s) > 0;
    }

    /* Validate contact: exactly 11 digits */
    static bool isValidContact(const char* s) {
        return myStrlen(s) == 11 && isAllDigits(s);
    }

    /* Validate password: min 6 chars */
    static bool isValidPassword(const char* s) {
        return myStrlen(s) >= 6;
    }

    /* Validate positive float > 0 */
    static bool isPositiveFloat(const char* s) {
        if (!s || !s[0]) return false;
        bool hasDot = false;
        int i = 0;
        if (s[i] == '-') return false;  // negative not allowed
        for (; s[i]; i++) {
            if (s[i] == '.') {
                if (hasDot) return false;
                hasDot = true;
            } else if (s[i] < '0' || s[i] > '9') return false;
        }
        return myAtof(s) > 0.0f;
    }

    /* Validate date DD-MM-YYYY, year >= currentYear */
    static bool isValidDate(const char* s) {
        if (myStrlen(s) != 10) return false;
        if (s[2] != '-' || s[5] != '-') return false;
        char d[3], m[3], y[5];
        myStrncpy(d, s,     3);
        myStrncpy(m, s + 3, 3);
        myStrncpy(y, s + 6, 5);
        if (!isAllDigits(d) || !isAllDigits(m) || !isAllDigits(y)) return false;
        int day = myAtoi(d), mon = myAtoi(m), yr = myAtoi(y);
        if (day < 1 || day > 31) return false;
        if (mon < 1 || mon > 12) return false;
        // year must be current year or later
        time_t t = time(nullptr);
        struct tm* ti = localtime(&t);
        int curYear = ti->tm_year + 1900;
        if (yr < curYear) return false;
        return true;
    }

    /* Validate time slot — one of 8 fixed slots */
    static bool isValidSlot(const char* s) {
        const char* slots[] = {"09:00","10:00","11:00","12:00",
                               "13:00","14:00","15:00","16:00"};
        for (int i = 0; i < 8; i++)
            if (myStrcmp(s, slots[i]) == 0) return true;
        return false;
    }

    /* Validate menu choice in range [lo, hi] */
    static bool isValidMenuChoice(int choice, int lo, int hi) {
        return choice >= lo && choice <= hi;
    }

    /* Validate ID > 0 */
    static bool isValidId(int id) { return id > 0; }
};

// ============================================================
//  SECTION 4 : Storage<T>  (generic template, dynamic array)
// ============================================================

template <typename T>
class Storage {
    T** data;       // dynamic array of pointers
    int capacity;
    int count;

    void resize() {
        capacity *= 2;
        T** newData = new T*[capacity];
        for (int i = 0; i < count; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
    }

public:
    Storage() : capacity(100), count(0) {
        data = new T*[capacity];
    }

    ~Storage() {
        // data pointers are owned by callers (FileHandler loads them)
        // We just free the pointer array, not the objects themselves
        // (objects freed separately)
        delete[] data;
    }

    void add(T* item) {
        if (count >= capacity) resize();
        data[count++] = item;
    }

    /* Remove by index (caller must delete the object) */
    void removeAt(int idx) {
        if (idx < 0 || idx >= count) return;
        for (int i = idx; i < count - 1; i++) data[i] = data[i+1];
        count--;
    }

    int size() const { return count; }

    T* get(int idx) const {
        if (idx < 0 || idx >= count) return nullptr;
        return data[idx];
    }

    /* Find by ID — requires T to have getId() */
    T* findById(int id) const {
        for (int i = 0; i < count; i++)
            if (data[i]->getId() == id) return data[i];
        return nullptr;
    }

    /* Remove by ID — returns pointer so caller can delete */
    T* removeById(int id) {
        for (int i = 0; i < count; i++) {
            if (data[i]->getId() == id) {
                T* tmp = data[i];
                removeAt(i);
                return tmp;
            }
        }
        return nullptr;
    }

    /* Give access to all — returns copy of internal array */
    T** getAll() const { return data; }
};

// ============================================================
//  SECTION 5 : Domain Classes
// ============================================================

/* ---------- Person (abstract base) ---------- */
class Person {
protected:
    int    id;
    char   name[60];
    char   contact[15];
    char   password[50];
public:
    Person() : id(0) { name[0] = contact[0] = password[0] = '\0'; }
    virtual ~Person() {}

    int         getId()      const { return id; }
    const char* getName()    const { return name; }
    const char* getContact() const { return contact; }
    const char* getPassword()const { return password; }

    void setId(int i)              { id = i; }
    void setName(const char* n)    { myStrncpy(name, n, 60); }
    void setContact(const char* c) { myStrncpy(contact, c, 15); }
    void setPassword(const char* p){ myStrncpy(password, p, 50); }

    // Pure virtual — subclasses must implement
    virtual void display()    const = 0;
    virtual void toFileLine(char* buf) const = 0;
};

/* ---------- Patient ---------- */
class Patient : public Person {
    int   age;
    char  gender[5];
    float balance;
public:
    Patient() : age(0), balance(0.0f) { gender[0] = '\0'; }

    int         getAge()     const { return age; }
    const char* getGender()  const { return gender; }
    float       getBalance() const { return balance; }

    void setAge(int a)             { age = a; }
    void setGender(const char* g)  { myStrncpy(gender, g, 5); }
    void setBalance(float b)       { balance = b; }

    /* += add to balance */
    Patient& operator+=(float amount) { balance += amount; return *this; }
    /* -= deduct from balance */
    Patient& operator-=(float amount) { balance -= amount; return *this; }
    /* == compare by ID */
    bool operator==(const Patient& o) const { return id == o.id; }

    /* << display */
    friend ostream& operator<<(ostream& os, const Patient& p) {
        char bal[20]; myFtoa(p.balance, bal);
        os << "ID: " << p.id
           << " | Name: " << p.name
           << " | Age: " << p.age
           << " | Gender: " << p.gender
           << " | Contact: " << p.contact
           << " | Balance: PKR " << bal;
        return os;
    }

    void display() const override {
        char bal[20]; myFtoa(balance, bal);
        cout << "ID: " << id
             << " | Name: " << name
             << " | Age: " << age
             << " | Gender: " << gender
             << " | Contact: " << contact
             << " | Balance: PKR " << bal << "\n";
    }

    /* Serialize to CSV line */
    void toFileLine(char* buf) const override {
        char idBuf[10], ageBuf[10], balBuf[20];
        myItoa(id, idBuf); myItoa(age, ageBuf); myFtoa(balance, balBuf);
        buf[0] = '\0';
        myStrcat(buf, idBuf); myStrcat(buf, ",");
        myStrcat(buf, name);  myStrcat(buf, ",");
        myStrcat(buf, ageBuf);myStrcat(buf, ",");
        myStrcat(buf, gender);myStrcat(buf, ",");
        myStrcat(buf, contact);myStrcat(buf, ",");
        myStrcat(buf, password);myStrcat(buf, ",");
        myStrcat(buf, balBuf);
    }
};

/* ---------- Doctor ---------- */
class Doctor : public Person {
    char  specialization[60];
    float fee;
public:
    Doctor() : fee(0.0f) { specialization[0] = '\0'; }

    const char* getSpec() const { return specialization; }
    float       getFee()  const { return fee; }

    void setSpec(const char* s) { myStrncpy(specialization, s, 60); }
    void setFee(float f)        { fee = f; }

    /* == compare by ID */
    bool operator==(const Doctor& o) const { return id == o.id; }

    /* << display */
    friend ostream& operator<<(ostream& os, const Doctor& d) {
        char feeBuf[20]; myFtoa(d.fee, feeBuf);
        os << "ID: " << d.id
           << " | Name: " << d.name
           << " | Spec: " << d.specialization
           << " | Contact: " << d.contact
           << " | Fee: PKR " << feeBuf;
        return os;
    }

    void display() const override {
        char feeBuf[20]; myFtoa(fee, feeBuf);
        cout << "ID: " << id
             << " | Name: " << name
             << " | Spec: " << specialization
             << " | Contact: " << contact
             << " | Fee: PKR " << feeBuf << "\n";
    }

    void toFileLine(char* buf) const override {
        char idBuf[10], feeBuf[20];
        myItoa(id, idBuf); myFtoa(fee, feeBuf);
        buf[0] = '\0';
        myStrcat(buf, idBuf); myStrcat(buf, ",");
        myStrcat(buf, name);  myStrcat(buf, ",");
        myStrcat(buf, specialization); myStrcat(buf, ",");
        myStrcat(buf, contact); myStrcat(buf, ",");
        myStrcat(buf, password); myStrcat(buf, ",");
        myStrcat(buf, feeBuf);
    }
};

/* ---------- Admin ---------- */
class Admin : public Person {
public:
    Admin() {}

    void display() const override {
        cout << "Admin ID: " << id << " | Name: " << name << "\n";
    }

    void toFileLine(char* buf) const override {
        char idBuf[10]; myItoa(id, idBuf);
        buf[0] = '\0';
        myStrcat(buf, idBuf); myStrcat(buf, ",");
        myStrcat(buf, name);  myStrcat(buf, ",");
        myStrcat(buf, password);
    }
};

/* ---------- Appointment ---------- */
class Appointment {
    int  id, patientId, doctorId;
    char date[12];
    char timeSlot[8];
    char status[15];   // pending / completed / cancelled / noshow
public:
    Appointment() : id(0), patientId(0), doctorId(0) {
        date[0] = timeSlot[0] = status[0] = '\0';
    }

    int         getId()        const { return id; }
    int         getPatientId() const { return patientId; }
    int         getDoctorId()  const { return doctorId; }
    const char* getDate()      const { return date; }
    const char* getSlot()      const { return timeSlot; }
    const char* getStatus()    const { return status; }

    void setId(int i)               { id = i; }
    void setPatientId(int i)        { patientId = i; }
    void setDoctorId(int i)         { doctorId = i; }
    void setDate(const char* d)     { myStrncpy(date, d, 12); }
    void setSlot(const char* s)     { myStrncpy(timeSlot, s, 8); }
    void setStatus(const char* s)   { myStrncpy(status, s, 15); }

    /* == detect scheduling conflict:
       same doctor, same date, same slot, neither is cancelled */
    bool operator==(const Appointment& o) const {
        if (doctorId != o.doctorId) return false;
        if (myStrcmp(date, o.date) != 0) return false;
        if (myStrcmp(timeSlot, o.timeSlot) != 0) return false;
        if (myStrcmp(status, "cancelled") == 0) return false;
        if (myStrcmp(o.status, "cancelled") == 0) return false;
        return true;
    }

    /* << display */
    friend ostream& operator<<(ostream& os, const Appointment& a) {
        os << "AppID: " << a.id
           << " | PatID: " << a.patientId
           << " | DocID: " << a.doctorId
           << " | Date: " << a.date
           << " | Slot: " << a.timeSlot
           << " | Status: " << a.status;
        return os;
    }

    void toFileLine(char* buf) const {
        char idBuf[10], pBuf[10], dBuf[10];
        myItoa(id, idBuf); myItoa(patientId, pBuf); myItoa(doctorId, dBuf);
        buf[0] = '\0';
        myStrcat(buf, idBuf); myStrcat(buf, ",");
        myStrcat(buf, pBuf);  myStrcat(buf, ",");
        myStrcat(buf, dBuf);  myStrcat(buf, ",");
        myStrcat(buf, date);  myStrcat(buf, ",");
        myStrcat(buf, timeSlot); myStrcat(buf, ",");
        myStrcat(buf, status);
    }
};

/* ---------- Bill ---------- */
class Bill {
    int   id, patientId, appointmentId;
    float amount;
    char  status[15];   // unpaid / paid / cancelled
    char  date[12];
public:
    Bill() : id(0), patientId(0), appointmentId(0), amount(0.0f) {
        status[0] = date[0] = '\0';
    }

    int         getId()           const { return id; }
    int         getPatientId()    const { return patientId; }
    int         getAppointmentId()const { return appointmentId; }
    float       getAmount()       const { return amount; }
    const char* getStatus()       const { return status; }
    const char* getDate()         const { return date; }

    void setId(int i)               { id = i; }
    void setPatientId(int i)        { patientId = i; }
    void setAppointmentId(int i)    { appointmentId = i; }
    void setAmount(float a)         { amount = a; }
    void setStatus(const char* s)   { myStrncpy(status, s, 15); }
    void setDate(const char* d)     { myStrncpy(date, d, 12); }

    void toFileLine(char* buf) const {
        char idB[10], pB[10], aB[10], amtB[20];
        myItoa(id, idB); myItoa(patientId, pB);
        myItoa(appointmentId, aB); myFtoa(amount, amtB);
        buf[0] = '\0';
        myStrcat(buf, idB);  myStrcat(buf, ",");
        myStrcat(buf, pB);   myStrcat(buf, ",");
        myStrcat(buf, aB);   myStrcat(buf, ",");
        myStrcat(buf, amtB); myStrcat(buf, ",");
        myStrcat(buf, status); myStrcat(buf, ",");
        myStrcat(buf, date);
    }
};

/* ---------- Prescription ---------- */
class Prescription {
    int  id, appointmentId, patientId, doctorId;
    char date[12];
    char medicines[500];
    char notes[300];
public:
    Prescription() : id(0), appointmentId(0), patientId(0), doctorId(0) {
        date[0] = medicines[0] = notes[0] = '\0';
    }

    int         getId()           const { return id; }
    int         getAppointmentId()const { return appointmentId; }
    int         getPatientId()    const { return patientId; }
    int         getDoctorId()     const { return doctorId; }
    const char* getDate()         const { return date; }
    const char* getMedicines()    const { return medicines; }
    const char* getNotes()        const { return notes; }

    void setId(int i)               { id = i; }
    void setAppointmentId(int i)    { appointmentId = i; }
    void setPatientId(int i)        { patientId = i; }
    void setDoctorId(int i)         { doctorId = i; }
    void setDate(const char* d)     { myStrncpy(date, d, 12); }
    void setMedicines(const char* m){ myStrncpy(medicines, m, 500); }
    void setNotes(const char* n)    { myStrncpy(notes, n, 300); }

    void toFileLine(char* buf) const {
        char idB[10], aB[10], pB[10], dB[10];
        myItoa(id, idB); myItoa(appointmentId, aB);
        myItoa(patientId, pB); myItoa(doctorId, dB);
        buf[0] = '\0';
        myStrcat(buf, idB);  myStrcat(buf, ",");
        myStrcat(buf, aB);   myStrcat(buf, ",");
        myStrcat(buf, pB);   myStrcat(buf, ",");
        myStrcat(buf, dB);   myStrcat(buf, ",");
        myStrcat(buf, date); myStrcat(buf, ",");
        myStrcat(buf, medicines); myStrcat(buf, ",");
        myStrcat(buf, notes);
    }
};

// ============================================================
//  SECTION 6 : FileHandler
// ============================================================

/*
 * FileHandler is the ONLY class that does file I/O.
 * It loads data into Storage<T>, appends, updates and deletes lines.
 */
class FileHandler {
public:

    /* ---- helpers ---- */

    /* Count lines in a file */
    static int countLines(const char* filename) {
        ifstream f(filename);
        if (!f.is_open()) return 0;
        int c = 0; char line[1024];
        while (f.getline(line, 1024)) if (myStrlen(line) > 0) c++;
        return c;
    }

    /* Get maximum integer ID from field 0 of a file */
    static int getMaxId(const char* filename) {
        ifstream f(filename);
        if (!f.is_open()) return 0;
        int maxId = 0; char line[1024], field[20];
        while (f.getline(line, 1024)) {
            if (!myStrlen(line)) continue;
            getField(line, 0, field, 20);
            int id = myAtoi(field);
            if (id > maxId) maxId = id;
        }
        return maxId;
    }

    /* Append a single line to a file */
    static void appendLine(const char* filename, const char* line) {
        ofstream f(filename, ios::app);
        if (f.is_open()) f << line << "\n";
    }

    /* Rewrite entire file from a char** lines array (n lines) */
    static void rewriteFile(const char* filename, char** lines, int n) {
        ofstream f(filename);
        if (!f.is_open()) return;
        for (int i = 0; i < n; i++)
            if (myStrlen(lines[i]) > 0) f << lines[i] << "\n";
    }

    /* Load all lines from a file into dynamic char** array.
       Caller must free with freeLines(). */
    static char** loadLines(const char* filename, int& n) {
        n = countLines(filename);
        if (n == 0) return nullptr;
        char** arr = new char*[n];
        ifstream f(filename);
        int idx = 0; char line[2048];
        while (f.getline(line, 2048) && idx < n) {
            if (!myStrlen(line)) { n--; continue; }
            arr[idx] = new char[myStrlen(line)+1];
            myStrcpy(arr[idx], line);
            idx++;
        }
        n = idx;
        return arr;
    }

    static void freeLines(char** lines, int n) {
        if (!lines) return;
        for (int i = 0; i < n; i++) delete[] lines[i];
        delete[] lines;
    }

    /* ---- Update a record by ID (field 0) ---- */
    static void updateRecordById(const char* filename, int id,
                                  const char* newLine) {
        int n; char** lines = loadLines(filename, n);
        if (!lines) return;
        char field[20];
        for (int i = 0; i < n; i++) {
            getField(lines[i], 0, field, 20);
            if (myAtoi(field) == id) {
                delete[] lines[i];
                lines[i] = new char[myStrlen(newLine)+1];
                myStrcpy(lines[i], newLine);
                break;
            }
        }
        rewriteFile(filename, lines, n);
        freeLines(lines, n);
    }

    /* ---- Delete a record by ID (field 0) ---- */
    static void deleteRecordById(const char* filename, int id) {
        int n; char** lines = loadLines(filename, n);
        if (!lines) return;
        char field[20];
        // Build new lines without the deleted one
        char** newLines = new char*[n];
        int newN = 0;
        for (int i = 0; i < n; i++) {
            getField(lines[i], 0, field, 20);
            if (myAtoi(field) == id) {
                delete[] lines[i]; // skip this
            } else {
                newLines[newN++] = lines[i];
            }
        }
        rewriteFile(filename, newLines, newN);
        // Free only the pointer array (strings freed above or kept)
        delete[] newLines;
        delete[] lines;   // already freed individual strings
    }

    /* ---- Delete all records where field fieldIdx == value ---- */
    static void deleteRecordsByField(const char* filename, int fieldIdx,
                                      const char* value) {
        int n; char** lines = loadLines(filename, n);
        if (!lines) return;
        char field[100];
        char** newLines = new char*[n];
        int newN = 0;
        for (int i = 0; i < n; i++) {
            getField(lines[i], fieldIdx, field, 100);
            if (myStrcmp(field, value) == 0) {
                delete[] lines[i];
            } else {
                newLines[newN++] = lines[i];
            }
        }
        rewriteFile(filename, newLines, newN);
        delete[] newLines;
        delete[] lines;
    }

    /* ---- Update a specific field in a record identified by field 0 ---- */
    static void updateField(const char* filename, int id,
                             int fieldIdx, const char* newValue) {
        int n; char** lines = loadLines(filename, n);
        if (!lines) return;
        char f0[20];
        for (int i = 0; i < n; i++) {
            getField(lines[i], 0, f0, 20);
            if (myAtoi(f0) != id) continue;
            // Rebuild line with updated field
            // Count total fields
            int total = countFields(lines[i]);
            char newLine[2048]; newLine[0] = '\0';
            for (int j = 0; j < total; j++) {
                if (j > 0) myStrcat(newLine, ",");
                if (j == fieldIdx) {
                    myStrcat(newLine, newValue);
                } else {
                    char tmp[500]; getField(lines[i], j, tmp, 500);
                    myStrcat(newLine, tmp);
                }
            }
            delete[] lines[i];
            lines[i] = new char[myStrlen(newLine)+1];
            myStrcpy(lines[i], newLine);
            break;
        }
        rewriteFile(filename, lines, n);
        freeLines(lines, n);
    }

    /* ---- Load Patients ---- */
    static void loadPatients(Storage<Patient>& store) {
        int n; char** lines = loadLines("patients.txt", n);
        if (!lines) return;
        for (int i = 0; i < n; i++) {
            char f[100];
            Patient* p = new Patient();
            getField(lines[i], 0, f, 100); p->setId(myAtoi(f));
            getField(lines[i], 1, f, 100); p->setName(f);
            getField(lines[i], 2, f, 100); p->setAge(myAtoi(f));
            getField(lines[i], 3, f, 100); p->setGender(f);
            getField(lines[i], 4, f, 100); p->setContact(f);
            getField(lines[i], 5, f, 100); p->setPassword(f);
            getField(lines[i], 6, f, 100); p->setBalance(myAtof(f));
            store.add(p);
        }
        freeLines(lines, n);
    }

    /* ---- Load Doctors ---- */
    static void loadDoctors(Storage<Doctor>& store) {
        int n; char** lines = loadLines("doctors.txt", n);
        if (!lines) return;
        for (int i = 0; i < n; i++) {
            char f[100];
            Doctor* d = new Doctor();
            getField(lines[i], 0, f, 100); d->setId(myAtoi(f));
            getField(lines[i], 1, f, 100); d->setName(f);
            getField(lines[i], 2, f, 100); d->setSpec(f);
            getField(lines[i], 3, f, 100); d->setContact(f);
            getField(lines[i], 4, f, 100); d->setPassword(f);
            getField(lines[i], 5, f, 100); d->setFee(myAtof(f));
            store.add(d);
        }
        freeLines(lines, n);
    }

    /* ---- Load Admin ---- */
    static Admin* loadAdmin() {
        int n; char** lines = loadLines("admin.txt", n);
        if (!lines || n == 0) return nullptr;
        char f[100];
        Admin* a = new Admin();
        getField(lines[0], 0, f, 100); a->setId(myAtoi(f));
        getField(lines[0], 1, f, 100); a->setName(f);
        getField(lines[0], 2, f, 100); a->setPassword(f);
        freeLines(lines, n);
        return a;
    }

    /* ---- Load Appointments ---- */
    static void loadAppointments(Storage<Appointment>& store) {
        int n; char** lines = loadLines("appointments.txt", n);
        if (!lines) return;
        for (int i = 0; i < n; i++) {
            char f[100];
            Appointment* a = new Appointment();
            getField(lines[i], 0, f, 100); a->setId(myAtoi(f));
            getField(lines[i], 1, f, 100); a->setPatientId(myAtoi(f));
            getField(lines[i], 2, f, 100); a->setDoctorId(myAtoi(f));
            getField(lines[i], 3, f, 100); a->setDate(f);
            getField(lines[i], 4, f, 100); a->setSlot(f);
            getField(lines[i], 5, f, 100); a->setStatus(f);
            store.add(a);
        }
        freeLines(lines, n);
    }

    /* ---- Load Bills ---- */
    static void loadBills(Storage<Bill>& store) {
        int n; char** lines = loadLines("bills.txt", n);
        if (!lines) return;
        for (int i = 0; i < n; i++) {
            char f[100];
            Bill* b = new Bill();
            getField(lines[i], 0, f, 100); b->setId(myAtoi(f));
            getField(lines[i], 1, f, 100); b->setPatientId(myAtoi(f));
            getField(lines[i], 2, f, 100); b->setAppointmentId(myAtoi(f));
            getField(lines[i], 3, f, 100); b->setAmount(myAtof(f));
            getField(lines[i], 4, f, 100); b->setStatus(f);
            getField(lines[i], 5, f, 100); b->setDate(f);
            store.add(b);
        }
        freeLines(lines, n);
    }

    /* ---- Load Prescriptions ---- */
    static void loadPrescriptions(Storage<Prescription>& store) {
        int n; char** lines = loadLines("prescriptions.txt", n);
        if (!lines) return;
        for (int i = 0; i < n; i++) {
            char f[600];
            Prescription* p = new Prescription();
            getField(lines[i], 0, f, 100); p->setId(myAtoi(f));
            getField(lines[i], 1, f, 100); p->setAppointmentId(myAtoi(f));
            getField(lines[i], 2, f, 100); p->setPatientId(myAtoi(f));
            getField(lines[i], 3, f, 100); p->setDoctorId(myAtoi(f));
            getField(lines[i], 4, f, 100); p->setDate(f);
            getField(lines[i], 5, f, 500); p->setMedicines(f);
            getField(lines[i], 6, f, 300); p->setNotes(f);
            store.add(p);
        }
        freeLines(lines, n);
    }

    /* ---- Ensure file exists (create if not) ---- */
    static void ensureFile(const char* filename) {
        ifstream f(filename);
        if (!f.is_open()) {
            ofstream create(filename);
            // file created empty
        }
    }

    /* ---- Log security event ---- */
    static void logSecurity(const char* role, const char* enteredId,
                              const char* result) {
        char ts[30]; getTimestamp(ts);
        char line[300]; line[0] = '\0';
        myStrcat(line, ts);        myStrcat(line, ",");
        myStrcat(line, role);      myStrcat(line, ",");
        myStrcat(line, enteredId); myStrcat(line, ",");
        myStrcat(line, result);
        appendLine("security_log.txt", line);
    }

    /* ---- Copy lines from src to dst that match field value ---- */
    static void copyMatchingLines(const char* src, const char* dst,
                                   int fieldIdx, const char* value) {
        int n; char** lines = loadLines(src, n);
        if (!lines) return;
        char f[100];
        for (int i = 0; i < n; i++) {
            getField(lines[i], fieldIdx, f, 100);
            if (myStrcmp(f, value) == 0) appendLine(dst, lines[i]);
        }
        freeLines(lines, n);
    }
};

// ============================================================
//  SECTION 7 : Sorting helpers (manual bubble sort)
// ============================================================

/* Sort appointment pointers by date ascending */
void sortAppointmentsByDateAsc(Appointment** arr, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (myStrcmp(arr[j]->getDate(), arr[j+1]->getDate()) > 0) {
                Appointment* tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
}

/* Sort appointment pointers by date descending */
void sortAppointmentsByDateDesc(Appointment** arr, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (myStrcmp(arr[j]->getDate(), arr[j+1]->getDate()) < 0) {
                Appointment* tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
}

/* Sort appointment pointers by time slot ascending */
void sortBySlotAsc(Appointment** arr, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (myStrcmp(arr[j]->getSlot(), arr[j+1]->getSlot()) > 0) {
                Appointment* tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
}

/* Sort prescription pointers by date descending */
void sortPrescByDateDesc(Prescription** arr, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (myStrcmp(arr[j]->getDate(), arr[j+1]->getDate()) < 0) {
                Prescription* tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
}

// ============================================================
//  SECTION 8 : Patient Menu Functions
// ============================================================

void patientBookAppointment(Patient* pat,
                             Storage<Doctor>& doctors,
                             Storage<Appointment>& appointments,
                             Storage<Bill>& bills) {
    // Ask for specialization
    char spec[60];
    cout << "Enter specialization to search: ";
    readLine(spec, 60);

    // Find matching doctors (case-insensitive)
    Doctor** matched = new Doctor*[doctors.size()];
    int matchCount = 0;
    for (int i = 0; i < doctors.size(); i++) {
        if (myStrcmpi(doctors.get(i)->getSpec(), spec) == 0)
            matched[matchCount++] = doctors.get(i);
    }
    if (matchCount == 0) {
        cout << "No doctors available for that specialization.\n";
        delete[] matched; return;
    }
    cout << "\nAvailable Doctors:\n";
    for (int i = 0; i < matchCount; i++) {
        char fee[20]; myFtoa(matched[i]->getFee(), fee);
        cout << "  ID: " << matched[i]->getId()
             << " | " << matched[i]->getName()
             << " | Fee: PKR " << fee << "\n";
    }

    // Get doctor ID
    cout << "Enter Doctor ID: ";
    char docIdBuf[20]; readLine(docIdBuf, 20);
    int docId = myAtoi(docIdBuf);
    Doctor* doc = doctors.findById(docId);
    if (!doc) { cout << "Doctor not found.\n"; delete[] matched; return; }

    // Validate doctor is in matched list
    bool inList = false;
    for (int i = 0; i < matchCount; i++)
        if (matched[i]->getId() == docId) { inList = true; break; }
    if (!inList) { cout << "Doctor not found.\n"; delete[] matched; return; }
    delete[] matched;

    // Get and validate date (max 3 attempts)
    char date[12];
    int dateAttempts = 0;
    while (true) {
        cout << "Enter date (DD-MM-YYYY): ";
        readLine(date, 12);
        if (Validator::isValidDate(date)) break;
        cout << "Invalid date. Use format DD-MM-YYYY.\n";
        dateAttempts++;
        if (dateAttempts >= 3) { cout << "Too many invalid attempts.\n"; return; }
    }

    // Show available slots
    const char* allSlots[] = {"09:00","10:00","11:00","12:00",
                               "13:00","14:00","15:00","16:00"};
    bool taken[8] = {false};
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (a->getDoctorId() == docId &&
            myStrcmp(a->getDate(), date) == 0 &&
            myStrcmp(a->getStatus(), "cancelled") != 0) {
            for (int s = 0; s < 8; s++)
                if (myStrcmp(a->getSlot(), allSlots[s]) == 0)
                    taken[s] = true;
        }
    }
    cout << "\nAvailable slots for " << date << ":\n";
    bool anyFree = false;
    for (int s = 0; s < 8; s++)
        if (!taken[s]) { cout << "  " << allSlots[s] << "\n"; anyFree = true; }
    if (!anyFree) { cout << "No slots available on this date.\n"; return; }

    // Get time slot (with re-display on conflict)
    char slot[8];
    while (true) {
        cout << "Enter time slot (e.g. 09:00): ";
        readLine(slot, 8);
        if (!Validator::isValidSlot(slot)) {
            cout << "Invalid slot. Choose from: 09:00 10:00 11:00 12:00 13:00 14:00 15:00 16:00\n";
            continue;
        }
        // Check if slot is taken
        bool slotTaken = false;
        for (int i = 0; i < appointments.size(); i++) {
            Appointment* a = appointments.get(i);
            if (a->getDoctorId() == docId &&
                myStrcmp(a->getDate(), date) == 0 &&
                myStrcmp(a->getSlot(), slot) == 0 &&
                myStrcmp(a->getStatus(), "cancelled") != 0) {
                slotTaken = true; break;
            }
        }
        if (slotTaken) {
            try { throw SlotUnavailableException("Slot unavailable. Please choose another."); }
            catch (SlotUnavailableException& e) {
                cout << e.what() << "\n";
                // Re-display available slots
                cout << "Available slots:\n";
                for (int s = 0; s < 8; s++)
                    if (!taken[s]) cout << "  " << allSlots[s] << "\n";
                continue;
            }
        }
        break;
    }

    // Check balance
    if (pat->getBalance() < doc->getFee()) {
        try { throw InsufficientFundsException("Insufficient funds. Please top up your balance."); }
        catch (InsufficientFundsException& e) {
            cout << e.what() << "\n"; return;
        }
    }

    // Deduct fee
    *pat -= doc->getFee();

    // Generate new appointment ID
    int newAppId = FileHandler::getMaxId("appointments.txt") + 1;

    // Create appointment object
    Appointment* newApp = new Appointment();
    newApp->setId(newAppId);
    newApp->setPatientId(pat->getId());
    newApp->setDoctorId(docId);
    newApp->setDate(date);
    newApp->setSlot(slot);
    newApp->setStatus("pending");

    // Write to file
    char appLine[300];
    newApp->toFileLine(appLine);
    FileHandler::appendLine("appointments.txt", appLine);
    appointments.add(newApp);

    // Generate bill
    int newBillId = FileHandler::getMaxId("bills.txt") + 1;
    Bill* newBill = new Bill();
    newBill->setId(newBillId);
    newBill->setPatientId(pat->getId());
    newBill->setAppointmentId(newAppId);
    newBill->setAmount(doc->getFee());
    newBill->setStatus("unpaid");
    newBill->setDate(date);

    char billLine[300];
    newBill->toFileLine(billLine);
    FileHandler::appendLine("bills.txt", billLine);
    bills.add(newBill);

    // Update patient balance in file
    char patLine[300];
    pat->toFileLine(patLine);
    FileHandler::updateRecordById("patients.txt", pat->getId(), patLine);

    char idBuf[10]; myItoa(newAppId, idBuf);
    cout << "Appointment booked successfully. Appointment ID: " << idBuf << "\n";
}

void patientCancelAppointment(Patient* pat,
                               Storage<Doctor>& doctors,
                               Storage<Appointment>& appointments) {
    // Collect pending appointments for this patient
    Appointment** pending = new Appointment*[appointments.size()];
    int pc = 0;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (a->getPatientId() == pat->getId() &&
            myStrcmp(a->getStatus(), "pending") == 0)
            pending[pc++] = a;
    }
    if (pc == 0) {
        cout << "You have no pending appointments.\n";
        delete[] pending; return;
    }
    cout << "\nPending Appointments:\n";
    cout << "AppID | Doctor Name | Date | Slot\n";
    for (int i = 0; i < pc; i++) {
        Doctor* d = doctors.findById(pending[i]->getDoctorId());
        cout << pending[i]->getId() << " | "
             << (d ? d->getName() : "Unknown") << " | "
             << pending[i]->getDate() << " | "
             << pending[i]->getSlot() << "\n";
    }

    cout << "Enter Appointment ID to cancel: ";
    int appId = readInt();

    // Validate it belongs to patient and is pending
    Appointment* target = nullptr;
    for (int i = 0; i < pc; i++)
        if (pending[i]->getId() == appId) { target = pending[i]; break; }

    if (!target) {
        cout << "Invalid appointment ID.\n"; delete[] pending; return;
    }
    delete[] pending;

    // Update status to cancelled
    target->setStatus("cancelled");
    char appLine[300]; target->toFileLine(appLine);
    FileHandler::updateRecordById("appointments.txt", appId, appLine);

    // Refund fee
    Doctor* doc = doctors.findById(target->getDoctorId());
    float fee = doc ? doc->getFee() : 0.0f;
    *pat += fee;

    // Update patient in file
    char patLine[300]; pat->toFileLine(patLine);
    FileHandler::updateRecordById("patients.txt", pat->getId(), patLine);

    // Update bill status to cancelled
    char appIdStr[10]; myItoa(appId, appIdStr);
    // Find bill with this appointment ID and update
    int n; char** lines = FileHandler::loadLines("bills.txt", n);
    if (lines) {
        char f2[50], f3[50];
        for (int i = 0; i < n; i++) {
            getField(lines[i], 1, f2, 50); // patient_id
            getField(lines[i], 2, f3, 50); // appointment_id
            if (myAtoi(f2) == pat->getId() && myAtoi(f3) == appId) {
                char f0[20]; getField(lines[i], 0, f0, 20);
                int billId = myAtoi(f0);
                FileHandler::updateField("bills.txt", billId, 4, "cancelled");
                break;
            }
        }
        FileHandler::freeLines(lines, n);
    }

    char feeBuf[20]; myFtoa(fee, feeBuf);
    cout << "Appointment cancelled. PKR " << feeBuf << " refunded to your balance.\n";
}

void patientViewAppointments(Patient* pat, Storage<Doctor>& doctors,
                              Storage<Appointment>& appointments) {
    Appointment** arr = new Appointment*[appointments.size()];
    int cnt = 0;
    for (int i = 0; i < appointments.size(); i++) {
        if (appointments.get(i)->getPatientId() == pat->getId())
            arr[cnt++] = appointments.get(i);
    }
    if (cnt == 0) { cout << "No appointments found.\n"; delete[] arr; return; }

    sortAppointmentsByDateAsc(arr, cnt);
    cout << "\nID | Doctor | Specialization | Date | Slot | Status\n";
    for (int i = 0; i < cnt; i++) {
        Doctor* d = doctors.findById(arr[i]->getDoctorId());
        cout << arr[i]->getId() << " | "
             << (d ? d->getName() : "?") << " | "
             << (d ? d->getSpec() : "?") << " | "
             << arr[i]->getDate() << " | "
             << arr[i]->getSlot() << " | "
             << arr[i]->getStatus() << "\n";
    }
    delete[] arr;
}

void patientViewMedicalRecords(Patient* pat, Storage<Doctor>& doctors,
                                Storage<Prescription>& prescriptions) {
    Prescription** arr = new Prescription*[prescriptions.size()];
    int cnt = 0;
    for (int i = 0; i < prescriptions.size(); i++)
        if (prescriptions.get(i)->getPatientId() == pat->getId())
            arr[cnt++] = prescriptions.get(i);

    if (cnt == 0) { cout << "No medical records found.\n"; delete[] arr; return; }
    sortPrescByDateDesc(arr, cnt);
    cout << "\nDate | Doctor | Medicines | Notes\n";
    for (int i = 0; i < cnt; i++) {
        Doctor* d = doctors.findById(arr[i]->getDoctorId());
        cout << arr[i]->getDate() << " | "
             << (d ? d->getName() : "?") << " | "
             << arr[i]->getMedicines() << " | "
             << arr[i]->getNotes() << "\n";
    }
    delete[] arr;
}

void patientViewBills(Patient* pat, Storage<Bill>& bills) {
    float total = 0;
    bool found = false;
    cout << "\nBill ID | Appointment ID | Amount | Status | Date\n";
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.get(i);
        if (b->getPatientId() != pat->getId()) continue;
        found = true;
        char amt[20]; myFtoa(b->getAmount(), amt);
        cout << b->getId() << " | "
             << b->getAppointmentId() << " | PKR "
             << amt << " | "
             << b->getStatus() << " | "
             << b->getDate() << "\n";
        if (myStrcmp(b->getStatus(), "unpaid") == 0) total += b->getAmount();
    }
    if (!found) { cout << "No bills found.\n"; return; }
    char tot[20]; myFtoa(total, tot);
    cout << "Total outstanding (unpaid): PKR " << tot << "\n";
}

void patientPayBill(Patient* pat, Storage<Bill>& bills) {
    // Show unpaid bills
    bool found = false;
    cout << "\nUnpaid Bills:\n";
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.get(i);
        if (b->getPatientId() == pat->getId() &&
            myStrcmp(b->getStatus(), "unpaid") == 0) {
            char amt[20]; myFtoa(b->getAmount(), amt);
            cout << "Bill ID: " << b->getId()
                 << " | Amount: PKR " << amt
                 << " | Date: " << b->getDate() << "\n";
            found = true;
        }
    }
    if (!found) { cout << "No unpaid bills.\n"; return; }

    cout << "Enter Bill ID to pay: ";
    int billId = readInt();

    Bill* target = nullptr;
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.get(i);
        if (b->getId() == billId && b->getPatientId() == pat->getId() &&
            myStrcmp(b->getStatus(), "unpaid") == 0)
            { target = b; break; }
    }
    if (!target) { cout << "Invalid bill ID.\n"; return; }

    if (pat->getBalance() < target->getAmount()) {
        try { throw InsufficientFundsException("Insufficient funds."); }
        catch (InsufficientFundsException& e) { cout << e.what() << "\n"; return; }
    }

    *pat -= target->getAmount();
    target->setStatus("paid");

    char billLine[300]; target->toFileLine(billLine);
    FileHandler::updateRecordById("bills.txt", billId, billLine);

    char patLine[300]; pat->toFileLine(patLine);
    FileHandler::updateRecordById("patients.txt", pat->getId(), patLine);

    char bal[20]; myFtoa(pat->getBalance(), bal);
    cout << "Bill paid successfully. Remaining balance: PKR " << bal << "\n";
}

void patientTopUp(Patient* pat) {
    int attempts = 0;
    while (attempts < 3) {
        cout << "Enter amount to add (PKR): ";
        char buf[50]; readLine(buf, 50);
        if (!Validator::isPositiveFloat(buf)) {
            try { throw InvalidInputException("Invalid amount. Must be a positive number."); }
            catch (InvalidInputException& e) {
                cout << e.what() << "\n"; attempts++; continue;
            }
        }
        float amt = myAtof(buf);
        *pat += amt;
        char patLine[300]; pat->toFileLine(patLine);
        FileHandler::updateRecordById("patients.txt", pat->getId(), patLine);
        char bal[20]; myFtoa(pat->getBalance(), bal);
        cout << "Balance updated. New balance: PKR " << bal << "\n";
        return;
    }
    cout << "Too many invalid attempts.\n";
}

// ----------------------------
// Patient session (login + menu)
// Split so SFML UI can authenticate without touching file handling.
// ----------------------------

void runPatientMenu(Patient* pat, Storage<Doctor>& doctors,
                    Storage<Appointment>& appointments,
                    Storage<Bill>& bills, Storage<Prescription>& prescriptions) {
    if (!pat) return;
    while (true) {
        char bal[20]; myFtoa(pat->getBalance(), bal);
        cout << "\nWelcome, " << pat->getName()
             << "\nBalance: PKR " << bal
             << "\n========================\n"
             << "1. Book Appointment\n"
             << "2. Cancel Appointment\n"
             << "3. View My Appointments\n"
             << "4. View My Medical Records\n"
             << "5. View My Bills\n"
             << "6. Pay Bill\n"
             << "7. Top Up Balance\n"
             << "8. Logout\n"
             << "Choice: ";
        int ch = readInt();
        switch (ch) {
            case 1: patientBookAppointment(pat, doctors, appointments, bills); break;
            case 2: patientCancelAppointment(pat, doctors, appointments); break;
            case 3: patientViewAppointments(pat, doctors, appointments); break;
            case 4: patientViewMedicalRecords(pat, doctors, prescriptions); break;
            case 5: patientViewBills(pat, bills); break;
            case 6: patientPayBill(pat, bills); break;
            case 7: patientTopUp(pat); break;
            case 8: cout << "Logged out.\n"; return;
            default: cout << "Invalid choice.\n";
        }
    }
}

Patient* loginPatient(Storage<Patient>& patients, int& failCount) {
    cout << "Enter Patient ID: ";
    int pid = readInt();
    cout << "Enter Password: ";
    char pw[50]; readLine(pw, 50);

    Patient* found = patients.findById(pid);
    if (found && myStrcmp(found->getPassword(), pw) == 0) return found;

    failCount++;
    char pidStr[10]; myItoa(pid, pidStr);
    FileHandler::logSecurity("Patient", pidStr, "FAILED");
    cout << "Invalid credentials. Attempts left: " << (3-failCount) << "\n";
    return nullptr;
}

Patient* loginPatientWithCredentials(Storage<Patient>& patients, int pid, const char* pw) {
    Patient* found = patients.findById(pid);
    if (found && myStrcmp(found->getPassword(), pw) == 0) return found;
    char pidStr[10]; myItoa(pid, pidStr);
    FileHandler::logSecurity("Patient", pidStr, "FAILED");
    return nullptr;
}

void runPatientSession(Storage<Patient>& patients, Storage<Doctor>& doctors,
                       Storage<Appointment>& appointments,
                       Storage<Bill>& bills, Storage<Prescription>& prescriptions) {
    int failCount = 0;
    Patient* pat = nullptr;
    while (failCount < 3 && !pat) pat = loginPatient(patients, failCount);
    if (!pat) { cout << "Account locked. Contact admin.\n"; return; }
    runPatientMenu(pat, doctors, appointments, bills, prescriptions);
}

// ============================================================
//  SECTION 9 : Doctor Menu Functions
// ============================================================

void doctorViewTodayAppointments(Doctor* doc, Storage<Patient>& patients,
                                   Storage<Appointment>& appointments) {
    char today[12]; getTodayDate(today);
    Appointment** arr = new Appointment*[appointments.size()];
    int cnt = 0;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (a->getDoctorId() == doc->getId() &&
            myStrcmp(a->getDate(), today) == 0)
            arr[cnt++] = a;
    }
    if (cnt == 0) {
        cout << "No appointments scheduled for today.\n";
        delete[] arr; return;
    }
    sortBySlotAsc(arr, cnt);
    cout << "\nAppID | Patient | Slot | Status\n";
    for (int i = 0; i < cnt; i++) {
        Patient* p = patients.findById(arr[i]->getPatientId());
        cout << arr[i]->getId() << " | "
             << (p ? p->getName() : "?") << " | "
             << arr[i]->getSlot() << " | "
             << arr[i]->getStatus() << "\n";
    }
    delete[] arr;
}

void doctorMarkComplete(Doctor* doc, Storage<Appointment>& appointments) {
    char today[12]; getTodayDate(today);
    // Show today's pending
    bool any = false;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (a->getDoctorId() == doc->getId() &&
            myStrcmp(a->getDate(), today) == 0 &&
            myStrcmp(a->getStatus(), "pending") == 0) {
            cout << "AppID: " << a->getId() << " | Slot: " << a->getSlot() << "\n";
            any = true;
        }
    }
    if (!any) { cout << "No pending appointments today.\n"; return; }

    cout << "Enter Appointment ID: ";
    int appId = readInt();
    Appointment* target = nullptr;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (a->getId() == appId && a->getDoctorId() == doc->getId() &&
            myStrcmp(a->getStatus(), "pending") == 0 &&
            myStrcmp(a->getDate(), today) == 0)
            { target = a; break; }
    }
    if (!target) { cout << "Invalid appointment ID.\n"; return; }

    target->setStatus("completed");
    char line[300]; target->toFileLine(line);
    FileHandler::updateRecordById("appointments.txt", appId, line);
    cout << "Appointment marked as completed.\n";
}

void doctorMarkNoShow(Doctor* doc, Storage<Appointment>& appointments) {
    char today[12]; getTodayDate(today);
    cout << "Enter Appointment ID: ";
    int appId = readInt();

    Appointment* target = nullptr;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (a->getId() == appId && a->getDoctorId() == doc->getId() &&
            myStrcmp(a->getStatus(), "pending") == 0 &&
            myStrcmp(a->getDate(), today) == 0)
            { target = a; break; }
    }
    if (!target) { cout << "Invalid appointment ID.\n"; return; }

    target->setStatus("noshow");
    char line[300]; target->toFileLine(line);
    FileHandler::updateRecordById("appointments.txt", appId, line);

    // Cancel corresponding bill (no refund)
    int n; char** lines = FileHandler::loadLines("bills.txt", n);
    if (lines) {
        char f2[50], f3[50];
        for (int i = 0; i < n; i++) {
            getField(lines[i], 2, f3, 50);
            if (myAtoi(f3) == appId) {
                char f0[20]; getField(lines[i], 0, f0, 20);
                FileHandler::updateField("bills.txt", myAtoi(f0), 4, "cancelled");
                break;
            }
        }
        FileHandler::freeLines(lines, n);
    }
    cout << "Appointment marked as no-show.\n";
}

void doctorWritePrescription(Doctor* doc, Storage<Appointment>& appointments,
                              Storage<Prescription>& prescriptions) {
    cout << "Enter Appointment ID: ";
    int appId = readInt();

    Appointment* target = nullptr;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (a->getId() == appId && a->getDoctorId() == doc->getId() &&
            myStrcmp(a->getStatus(), "completed") == 0)
            { target = a; break; }
    }
    if (!target) { cout << "Invalid or incomplete appointment.\n"; return; }

    // Check if prescription already exists
    for (int i = 0; i < prescriptions.size(); i++)
        if (prescriptions.get(i)->getAppointmentId() == appId) {
            cout << "Prescription already written for this appointment.\n"; return;
        }

    char medicines[500];
    cout << "Enter medicines (e.g. Paracetamol 500mg;Amoxicillin 250mg): ";
    readLine(medicines, 500);

    char notes[300];
    cout << "Enter notes (max 300 chars): ";
    readLine(notes, 300);

    int newId = FileHandler::getMaxId("prescriptions.txt") + 1;
    Prescription* p = new Prescription();
    p->setId(newId);
    p->setAppointmentId(appId);
    p->setPatientId(target->getPatientId());
    p->setDoctorId(doc->getId());
    p->setDate(target->getDate());
    p->setMedicines(medicines);
    p->setNotes(notes);

    char line[1200]; p->toFileLine(line);
    FileHandler::appendLine("prescriptions.txt", line);
    prescriptions.add(p);
    cout << "Prescription saved.\n";
}

void doctorViewPatientHistory(Doctor* doc, Storage<Patient>& patients,
                               Storage<Appointment>& appointments,
                               Storage<Prescription>& prescriptions) {
    cout << "Enter Patient ID: ";
    int pid = readInt();

    Patient* pat = patients.findById(pid);
    if (!pat) { cout << "Access denied. You can only view records of your own patients.\n"; return; }

    // Check at least one completed appointment with this doctor
    bool hasRecord = false;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (a->getPatientId() == pid && a->getDoctorId() == doc->getId() &&
            myStrcmp(a->getStatus(), "completed") == 0)
            { hasRecord = true; break; }
    }
    if (!hasRecord) {
        cout << "Access denied. You can only view records of your own patients.\n"; return;
    }

    // Gather prescriptions by this doctor for this patient
    Prescription** arr = new Prescription*[prescriptions.size()];
    int cnt = 0;
    for (int i = 0; i < prescriptions.size(); i++) {
        Prescription* p = prescriptions.get(i);
        if (p->getPatientId() == pid && p->getDoctorId() == doc->getId())
            arr[cnt++] = p;
    }
    if (cnt == 0) { cout << "No records found.\n"; delete[] arr; return; }
    sortPrescByDateDesc(arr, cnt);
    cout << "\nDate | Medicines | Notes\n";
    for (int i = 0; i < cnt; i++)
        cout << arr[i]->getDate() << " | " << arr[i]->getMedicines()
             << " | " << arr[i]->getNotes() << "\n";
    delete[] arr;
}

void runDoctorSession(Storage<Doctor>& doctors, Storage<Patient>& patients,
                       Storage<Appointment>& appointments, Storage<Bill>& bills,
                       Storage<Prescription>& prescriptions) {
    // (implementation moved below; wrapper kept for backward compatibility)
    int failCount = 0;
    Doctor* doc = nullptr;
    while (failCount < 3 && !doc) {
        cout << "Enter Doctor ID: ";
        int did = readInt();
        cout << "Enter Password: ";
        char pw[50]; readLine(pw, 50);

        Doctor* found = doctors.findById(did);
        if (found && myStrcmp(found->getPassword(), pw) == 0) {
            doc = found;
        } else {
            failCount++;
            char didStr[10]; myItoa(did, didStr);
            FileHandler::logSecurity("Doctor", didStr, "FAILED");
            cout << "Invalid credentials. Attempts left: " << (3-failCount) << "\n";
        }
    }
    if (!doc) { cout << "Account locked. Contact admin.\n"; return; }
    while (true) {
        cout << "\nWelcome, Dr. " << doc->getName()
             << " | Specialization: " << doc->getSpec()
             << "\n===============================================\n"
             << "1. View Today's Appointments\n"
             << "2. Mark Appointment Complete\n"
             << "3. Mark Appointment No-Show\n"
             << "4. Write Prescription\n"
             << "5. View Patient Medical History\n"
             << "6. Logout\n"
             << "Choice: ";
        int ch = readInt();
        switch (ch) {
            case 1: doctorViewTodayAppointments(doc, patients, appointments); break;
            case 2: doctorMarkComplete(doc, appointments); break;
            case 3: doctorMarkNoShow(doc, appointments); break;
            case 4: doctorWritePrescription(doc, appointments, prescriptions); break;
            case 5: doctorViewPatientHistory(doc, patients, appointments, prescriptions); break;
            case 6: cout << "Logged out.\n"; return;
            default: cout << "Invalid choice.\n";
        }
    }
}

Doctor* loginDoctorWithCredentials(Storage<Doctor>& doctors, int did, const char* pw) {
    Doctor* found = doctors.findById(did);
    if (found && myStrcmp(found->getPassword(), pw) == 0) return found;
    char didStr[10]; myItoa(did, didStr);
    FileHandler::logSecurity("Doctor", didStr, "FAILED");
    return nullptr;
}

// ============================================================
//  SECTION 10 : Admin Menu Functions
// ============================================================

/* Core add-doctor (file + memory) — used by console menu and SFML admin panel */
void adminAddDoctorCore(Storage<Doctor>& doctors, const char* name, const char* spec,
                        const char* contact, const char* pw, float fee) {
    int newId = FileHandler::getMaxId("doctors.txt") + 1;
    Doctor* d = new Doctor();
    d->setId(newId);
    d->setName(name);
    d->setSpec(spec);
    d->setContact(contact);
    d->setPassword(pw);
    d->setFee(fee);

    char line[300]; d->toFileLine(line);
    FileHandler::appendLine("doctors.txt", line);
    doctors.add(d);
}

void adminAddDoctor(Storage<Doctor>& doctors) {
    char name[60], spec[60], contact[15], pw[50], feeBuf[20];

    cout << "Enter name (max 50 chars): ";
    readLine(name, 60);

    cout << "Enter specialization (max 50 chars): ";
    readLine(spec, 60);

    // Validate contact
    while (true) {
        cout << "Enter contact (11 digits): ";
        readLine(contact, 15);
        if (Validator::isValidContact(contact)) break;
        cout << "Invalid. Must be exactly 11 digits.\n";
    }

    // Validate password
    while (true) {
        cout << "Enter password (min 6 chars): ";
        readLine(pw, 50);
        if (Validator::isValidPassword(pw)) break;
        cout << "Invalid. Min 6 characters.\n";
    }

    // Validate fee
    while (true) {
        cout << "Enter consultation fee: ";
        readLine(feeBuf, 20);
        if (Validator::isPositiveFloat(feeBuf)) break;
        cout << "Invalid. Must be a positive number.\n";
    }
    float fee = myAtof(feeBuf);

    adminAddDoctorCore(doctors, name, spec, contact, pw, fee);
    int newDoctorId = FileHandler::getMaxId("doctors.txt");
    char idBuf[10]; myItoa(newDoctorId, idBuf);
    cout << "Doctor added successfully. ID: " << idBuf << "\n";
}

/* Returns false and writes a short reason into err (if non-null) */
bool adminRemoveDoctorById(Storage<Doctor>& doctors,
                           Storage<Appointment>& appointments, int did,
                           char* err, int errLen) {
    if (err && errLen > 0) err[0] = '\0';
    Doctor* doc = doctors.findById(did);
    if (!doc) {
        if (err && errLen > 0) myStrncpy(err, "Doctor not found.", errLen);
        return false;
    }
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (a->getDoctorId() == did &&
            myStrcmp(a->getStatus(), "pending") == 0) {
            if (err && errLen > 0)
                myStrncpy(err, "Cannot remove: doctor has pending appointments.", errLen);
            return false;
        }
    }
    FileHandler::deleteRecordById("doctors.txt", did);
    Doctor* removed = doctors.removeById(did);
    delete removed;
    return true;
}

void adminRemoveDoctor(Storage<Doctor>& doctors,
                        Storage<Appointment>& appointments) {
    cout << "\nAll Doctors:\n";
    for (int i = 0; i < doctors.size(); i++) doctors.get(i)->display();

    cout << "Enter Doctor ID to remove: ";
    int did = readInt();

    char err[120];
    if (!adminRemoveDoctorById(doctors, appointments, did, err, 120))
        cout << err << "\n";
    else
        cout << "Doctor removed.\n";
}

void adminViewAllPatients(Storage<Patient>& patients, Storage<Bill>& bills) {
    cout << "\nID | Name | Age | Gender | Contact | Balance | Unpaid Bills\n";
    for (int i = 0; i < patients.size(); i++) {
        Patient* p = patients.get(i);
        int unpaid = 0;
        for (int j = 0; j < bills.size(); j++) {
            Bill* b = bills.get(j);
            if (b->getPatientId() == p->getId() &&
                myStrcmp(b->getStatus(), "unpaid") == 0) unpaid++;
        }
        char bal[20]; myFtoa(p->getBalance(), bal);
        cout << p->getId() << " | " << p->getName()
             << " | " << p->getAge()
             << " | " << p->getGender()
             << " | " << p->getContact()
             << " | PKR " << bal
             << " | " << unpaid << "\n";
    }
}

void adminViewAllDoctors(Storage<Doctor>& doctors) {
    cout << "\nID | Name | Specialization | Contact | Fee\n";
    for (int i = 0; i < doctors.size(); i++) doctors.get(i)->display();
}

void adminViewAllAppointments(Storage<Patient>& patients,
                               Storage<Doctor>& doctors,
                               Storage<Appointment>& appointments) {
    int n = appointments.size();
    if (n == 0) { cout << "No appointments.\n"; return; }
    Appointment** arr = new Appointment*[n];
    for (int i = 0; i < n; i++) arr[i] = appointments.get(i);
    sortAppointmentsByDateDesc(arr, n);

    cout << "\nID | Patient | Doctor | Date | Slot | Status\n";
    for (int i = 0; i < n; i++) {
        Patient* p = patients.findById(arr[i]->getPatientId());
        Doctor*  d = doctors.findById(arr[i]->getDoctorId());
        cout << arr[i]->getId() << " | "
             << (p ? p->getName() : "?") << " | "
             << (d ? d->getName() : "?") << " | "
             << arr[i]->getDate() << " | "
             << arr[i]->getSlot() << " | "
             << arr[i]->getStatus() << "\n";
    }
    delete[] arr;
}

void adminViewUnpaidBills(Storage<Patient>& patients, Storage<Bill>& bills) {
    char today[12]; getTodayDate(today);
    bool found = false;
    cout << "\nBill ID | Patient | Amount | Date\n";
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.get(i);
        if (myStrcmp(b->getStatus(), "unpaid") != 0) continue;
        found = true;
        Patient* p = patients.findById(b->getPatientId());
        char amt[20]; myFtoa(b->getAmount(), amt);
        int diff = daysBetween(b->getDate(), today);
        cout << b->getId() << " | "
             << (p ? p->getName() : "?") << " | PKR "
             << amt << " | " << b->getDate();
        if (diff > 7) cout << " [OVERDUE]";
        cout << "\n";
    }
    if (!found) cout << "No unpaid bills.\n";
}

bool adminDischargePatientById(Storage<Patient>& patients,
                               Storage<Doctor>& doctors,
                               Storage<Appointment>& appointments,
                               Storage<Bill>& bills,
                               Storage<Prescription>& prescriptions,
                               int pid, char* err, int errLen) {
    (void)doctors;
    (void)prescriptions;
    if (err && errLen > 0) err[0] = '\0';
    Patient* pat = patients.findById(pid);
    if (!pat) {
        if (err && errLen > 0) myStrncpy(err, "Patient not found.", errLen);
        return false;
    }
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.get(i);
        if (b->getPatientId() == pid && myStrcmp(b->getStatus(), "unpaid") == 0) {
            if (err && errLen > 0)
                myStrncpy(err, "Cannot discharge: patient has unpaid bills.", errLen);
            return false;
        }
    }
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (a->getPatientId() == pid && myStrcmp(a->getStatus(), "pending") == 0) {
            if (err && errLen > 0)
                myStrncpy(err, "Cannot discharge: pending appointments exist.", errLen);
            return false;
        }
    }
    char pidStr[10]; myItoa(pid, pidStr);
    FileHandler::copyMatchingLines("patients.txt", "discharged.txt", 0, pidStr);
    FileHandler::copyMatchingLines("appointments.txt", "discharged.txt", 1, pidStr);
    FileHandler::copyMatchingLines("prescriptions.txt", "discharged.txt", 2, pidStr);
    FileHandler::copyMatchingLines("bills.txt", "discharged.txt", 1, pidStr);

    FileHandler::deleteRecordById("patients.txt", pid);
    FileHandler::deleteRecordsByField("appointments.txt", 1, pidStr);
    FileHandler::deleteRecordsByField("prescriptions.txt", 2, pidStr);
    FileHandler::deleteRecordsByField("bills.txt", 1, pidStr);

    Patient* removed = patients.removeById(pid);
    delete removed;
    return true;
}

void adminDischargePatient(Storage<Patient>& patients,
                            Storage<Doctor>& doctors,
                            Storage<Appointment>& appointments,
                            Storage<Bill>& bills,
                            Storage<Prescription>& prescriptions) {
    cout << "Enter Patient ID: ";
    int pid = readInt();

    char err[160];
    if (!adminDischargePatientById(patients, doctors, appointments, bills, prescriptions, pid, err, 160))
        cout << err << "\n";
    else
        cout << "Patient discharged and archived successfully.\n";
}

void adminViewSecurityLog() {
    ifstream f("security_log.txt");
    if (!f.is_open()) { cout << "No security events logged.\n"; return; }
    char line[300];
    bool any = false;
    while (f.getline(line, 300)) {
        if (myStrlen(line) > 0) { cout << line << "\n"; any = true; }
    }
    if (!any) cout << "No security events logged.\n";
}

void adminGenerateDailyReport(Storage<Patient>& patients,
                               Storage<Doctor>& doctors,
                               Storage<Appointment>& appointments,
                               Storage<Bill>& bills) {
    char today[12]; getTodayDate(today);
    cout << "\n=== Daily Report for " << today << " ===\n";

    // Appointment stats
    int totalA=0, pendA=0, compA=0, noA=0, canA=0;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (myStrcmp(a->getDate(), today) != 0) continue;
        totalA++;
        if (myStrcmp(a->getStatus(),"pending")==0)   pendA++;
        else if (myStrcmp(a->getStatus(),"completed")==0) compA++;
        else if (myStrcmp(a->getStatus(),"noshow")==0) noA++;
        else if (myStrcmp(a->getStatus(),"cancelled")==0) canA++;
    }
    cout << "Total appointments today: " << totalA
         << " (Pending: " << pendA
         << " Completed: " << compA
         << " No-show: " << noA
         << " Cancelled: " << canA << ")\n";

    // Revenue: paid bills dated today
    float revenue = 0;
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.get(i);
        if (myStrcmp(b->getDate(), today)==0 &&
            myStrcmp(b->getStatus(),"paid")==0) revenue += b->getAmount();
    }
    char rev[20]; myFtoa(revenue, rev);
    cout << "Revenue collected today (paid bills): PKR " << rev << "\n";

    // Patients with unpaid bills
    cout << "\nPatients with outstanding unpaid bills:\n";
    cout << "Patient Name | Total Owed\n";
    for (int i = 0; i < patients.size(); i++) {
        Patient* p = patients.get(i);
        float owed = 0;
        for (int j = 0; j < bills.size(); j++) {
            Bill* b = bills.get(j);
            if (b->getPatientId()==p->getId() &&
                myStrcmp(b->getStatus(),"unpaid")==0) owed += b->getAmount();
        }
        if (owed > 0) {
            char ow[20]; myFtoa(owed, ow);
            cout << p->getName() << " | PKR " << ow << "\n";
        }
    }

    // Doctor-wise summary today
    cout << "\nDoctor-wise summary for today:\n";
    cout << "Doctor Name | Completed | Pending | No-show\n";
    for (int i = 0; i < doctors.size(); i++) {
        Doctor* d = doctors.get(i);
        int dc=0, dp=0, dn=0;
        for (int j = 0; j < appointments.size(); j++) {
            Appointment* a = appointments.get(j);
            if (a->getDoctorId()!=d->getId()) continue;
            if (myStrcmp(a->getDate(),today)!=0) continue;
            if (myStrcmp(a->getStatus(),"completed")==0) dc++;
            else if (myStrcmp(a->getStatus(),"pending")==0) dp++;
            else if (myStrcmp(a->getStatus(),"noshow")==0) dn++;
        }
        if (dc+dp+dn > 0)
            cout << d->getName() << " | " << dc << " | " << dp << " | " << dn << "\n";
    }
}

void runAdminSession(Admin* admin, Storage<Patient>& patients,
                      Storage<Doctor>& doctors,
                      Storage<Appointment>& appointments,
                      Storage<Bill>& bills,
                      Storage<Prescription>& prescriptions) {
    // Admin login
    int failCount = 0;
    bool loggedIn = false;
    while (failCount < 3) {
        cout << "Enter Admin ID: ";
        int aid = readInt();
        cout << "Enter Password: ";
        char pw[50]; readLine(pw, 50);

        if (admin->getId() == aid &&
            myStrcmp(admin->getPassword(), pw) == 0) {
            loggedIn = true; break;
        } else {
            failCount++;
            char aidStr[10]; myItoa(aid, aidStr);
            FileHandler::logSecurity("Admin", aidStr, "FAILED");
            cout << "Invalid credentials. Attempts left: " << (3-failCount) << "\n";
        }
    }
    if (!loggedIn) { cout << "Account locked. Contact admin.\n"; return; }

    while (true) {
        cout << "\nAdmin Panel - MediCore\n"
             << "======================\n"
             << "1. Add Doctor\n"
             << "2. Remove Doctor\n"
             << "3. View All Patients\n"
             << "4. View All Doctors\n"
             << "5. View All Appointments\n"
             << "6. View Unpaid Bills\n"
             << "7. Discharge Patient\n"
             << "8. View Security Log\n"
             << "9. Generate Daily Report\n"
             << "10. Logout\n"
             << "Choice: ";
        int ch = readInt();
        switch (ch) {
            case 1:  adminAddDoctor(doctors); break;
            case 2:  adminRemoveDoctor(doctors, appointments); break;
            case 3:  adminViewAllPatients(patients, bills); break;
            case 4:  adminViewAllDoctors(doctors); break;
            case 5:  adminViewAllAppointments(patients, doctors, appointments); break;
            case 6:  adminViewUnpaidBills(patients, bills); break;
            case 7:  adminDischargePatient(patients, doctors, appointments, bills, prescriptions); break;
            case 8:  adminViewSecurityLog(); break;
            case 9:  adminGenerateDailyReport(patients, doctors, appointments, bills); break;
            case 10: cout << "Logged out.\n"; return;
            default: cout << "Invalid choice.\n";
        }
    }
}

bool loginAdminWithCredentials(Admin* admin, int aid, const char* pw) {
    if (!admin) return false;
    if (admin->getId() == aid && myStrcmp(admin->getPassword(), pw) == 0) return true;
    char aidStr[10]; myItoa(aid, aidStr);
    FileHandler::logSecurity("Admin", aidStr, "FAILED");
    return false;
}

#ifdef USE_SFML_UI
// ============================================================
//  SFML UI: Hospital-themed login + in-window Admin dashboard.
//  Patient/Doctor: console is shown temporarily (menus still use cin).
//  File logic unchanged (calls same core/write paths as console admin).
// ============================================================

namespace UiTheme {
static const sf::Color BG_CLOUD(237, 245, 251);
static const sf::Color HEADER_TEAL(11, 91, 99);
static const sf::Color ACCENT_CYAN(0, 149, 135);
static const sf::Color CARD_WHITE(255, 255, 255);
static const sf::Color BORDER_SOFT(160, 210, 230);
static const sf::Color TEXT_DARK(33, 47, 62);
static const sf::Color TEXT_SOFT(71, 90, 105);
static const sf::Color ERROR_ROSE(198, 40, 40);
}

static void sfmlBufCat(char* buf, int cap, const char* s) {
    int bl = myStrlen(buf);
    int sl = myStrlen(s);
    if (bl + sl + 1 >= cap) return;
    for (int k = 0; k <= sl; k++) buf[bl + k] = s[k];
}

static bool sfmlTryLoadFont(sf::Font& font) {
    const char* candidates[] = {
        "arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttc",
        "C:\\Windows\\Fonts\\ARIAL.TTC",
        "C:\\Windows\\Fonts\\calibri.ttf",
        "C:\\Windows\\Fonts\\segoeui.ttf",
        "C:\\Windows\\Fonts\\SegoeUI.ttf"
    };
    for (int i = 0; i < (int)(sizeof(candidates) / sizeof(candidates[0])); i++)
        if (font.loadFromFile(candidates[i])) return true;
    return false;
}

static void sfmlBuildPatientsTxt(Storage<Patient>& patients, Storage<Bill>& bills, char* out, int cap) {
    out[0] = '\0';
    sfmlBufCat(out, cap, "ID | Name | Age | Gender | Contact | Balance | Unpaid\n");
    sfmlBufCat(out, cap, "---------------------------------------------------------------\n");
    for (int i = 0; i < patients.size(); i++) {
        Patient* p = patients.get(i);
        int unpaid = 0;
        for (int j = 0; j < bills.size(); j++) {
            Bill* b = bills.get(j);
            if (b->getPatientId() == p->getId() && myStrcmp(b->getStatus(), "unpaid") == 0) unpaid++;
        }
        char bal[20]; myFtoa(p->getBalance(), bal);
        char line[180];
        myItoa(p->getId(), line); myStrcat(line, " | ");
        myStrcat(line, p->getName()); myStrcat(line, " | ");
        char ab[16]; myItoa(p->getAge(), ab); myStrcat(line, ab); myStrcat(line, " | ");
        myStrcat(line, p->getGender()); myStrcat(line, " | ");
        myStrcat(line, p->getContact()); myStrcat(line, " | PKR ");
        myStrcat(line, bal); myStrcat(line, " | ");
        char ubuf[16]; myItoa(unpaid, ubuf); myStrcat(line, ubuf);
        sfmlBufCat(out, cap, line);
        sfmlBufCat(out, cap, "\n");
    }
}

static void sfmlBuildDoctorsTxt(Storage<Doctor>& doctors, char* out, int cap) {
    out[0] = '\0';
    sfmlBufCat(out, cap, "ID | Name | Specialization | Contact | Fee PKR\n");
    sfmlBufCat(out, cap, "------------------------------------------------\n");
    for (int i = 0; i < doctors.size(); i++) {
        Doctor* d = doctors.get(i);
        char fee[20]; myFtoa(d->getFee(), fee);
        char line[200];
        myItoa(d->getId(), line); myStrcat(line, " | ");
        myStrcat(line, d->getName()); myStrcat(line, " | ");
        myStrcat(line, d->getSpec()); myStrcat(line, " | ");
        myStrcat(line, d->getContact()); myStrcat(line, " | ");
        myStrcat(line, fee);
        sfmlBufCat(out, cap, line);
        sfmlBufCat(out, cap, "\n");
    }
}

static void sfmlBuildAppointmentsTxt(Storage<Patient>& patients,
    Storage<Doctor>& doctors,
    Storage<Appointment>& appointments, char* out, int cap) {
    out[0] = '\0';
    int n = appointments.size();
    if (n == 0) {
        sfmlBufCat(out, cap, "(No appointments.)\n");
        return;
    }
    Appointment** arr = new Appointment*[n];
    for (int i = 0; i < n; i++) arr[i] = appointments.get(i);
    sortAppointmentsByDateDesc(arr, n);

    sfmlBufCat(out, cap, "ID | Patient | Doctor | Date | Slot | Status\n");
    sfmlBufCat(out, cap, "---------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        Patient* pa = patients.findById(arr[i]->getPatientId());
        Doctor* doc = doctors.findById(arr[i]->getDoctorId());
        char line[240];
        myItoa(arr[i]->getId(), line); myStrcat(line, " | ");
        myStrcat(line, pa ? pa->getName() : "?");
        myStrcat(line, " | ");
        myStrcat(line, doc ? doc->getName() : "?");
        myStrcat(line, " | ");
        myStrcat(line, arr[i]->getDate());
        myStrcat(line, " | ");
        myStrcat(line, arr[i]->getSlot());
        myStrcat(line, " | ");
        myStrcat(line, arr[i]->getStatus());
        sfmlBufCat(out, cap, line);
        sfmlBufCat(out, cap, "\n");
    }
    delete[] arr;
}

static void sfmlBuildUnpaidBillsTxt(Storage<Patient>& patients, Storage<Bill>& bills,
    char* out, int cap) {
    out[0] = '\0';
    char today[12]; getTodayDate(today);
    sfmlBufCat(out, cap, "Bill ID | Patient | Amount PKR | Date\n");
    sfmlBufCat(out, cap, "---------------------------------------\n");
    bool found = false;
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.get(i);
        if (myStrcmp(b->getStatus(), "unpaid") != 0) continue;
        found = true;
        Patient* p = patients.findById(b->getPatientId());
        char amt[20]; myFtoa(b->getAmount(), amt);
        char line[200];
        myItoa(b->getId(), line); myStrcat(line, " | ");
        myStrcat(line, p ? p->getName() : "?");
        myStrcat(line, " | ");
        myStrcat(line, amt);
        myStrcat(line, " | ");
        myStrcat(line, b->getDate());
        int diff = daysBetween(b->getDate(), today);
        if (diff > 7) myStrcat(line, " [OVERDUE]");
        sfmlBufCat(out, cap, line);
        sfmlBufCat(out, cap, "\n");
    }
    if (!found) sfmlBufCat(out, cap, "(No unpaid bills.)\n");
}

static void sfmlBuildSecurityLogTxt(char* out, int cap) {
    out[0] = '\0';
    ifstream f("security_log.txt");
    if (!f.is_open()) {
        sfmlBufCat(out, cap, "(No security_log.txt opened.)\n");
        return;
    }
    char line[340];
    bool any = false;
    while (f.getline(line, 340)) {
        if (!myStrlen(line)) continue;
        any = true;
        sfmlBufCat(out, cap, line);
        sfmlBufCat(out, cap, "\n");
    }
    if (!any) sfmlBufCat(out, cap, "(Log empty.)\n");
}

static void sfmlBuildDailyReportTxt(Storage<Patient>& patients,
    Storage<Doctor>& doctors,
    Storage<Appointment>& appointments,
    Storage<Bill>& bills, char* out, int cap) {
    out[0] = '\0';
    char today[12]; getTodayDate(today);
    char hdr[120];
    hdr[0] = '\0';
    sfmlBufCat(hdr, sizeof(hdr), "=== Daily Report for ");
    sfmlBufCat(hdr, sizeof(hdr), today);
    sfmlBufCat(hdr, sizeof(hdr), " ===\n");
    sfmlBufCat(out, cap, hdr);

    int totalA = 0, pendA = 0, compA = 0, noA = 0, canA = 0;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.get(i);
        if (myStrcmp(a->getDate(), today) != 0) continue;
        totalA++;
        if (myStrcmp(a->getStatus(), "pending") == 0) pendA++;
        else if (myStrcmp(a->getStatus(), "completed") == 0) compA++;
        else if (myStrcmp(a->getStatus(), "noshow") == 0) noA++;
        else if (myStrcmp(a->getStatus(), "cancelled") == 0) canA++;
    }
    char buf[420];
    myItoa(totalA, buf);
    sfmlBufCat(out, cap, "Appointments today: ");
    sfmlBufCat(out, cap, buf);
    sfmlBufCat(out, cap, " (pending ");
    myItoa(pendA, buf); sfmlBufCat(out, cap, buf);
    sfmlBufCat(out, cap, ", done ");
    myItoa(compA, buf); sfmlBufCat(out, cap, buf);
    sfmlBufCat(out, cap, ", noshow ");
    myItoa(noA, buf); sfmlBufCat(out, cap, buf);
    sfmlBufCat(out, cap, ", cancelled ");
    myItoa(canA, buf); sfmlBufCat(out, cap, buf);
    sfmlBufCat(out, cap, ")\n");

    float revenue = 0;
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.get(i);
        if (myStrcmp(b->getDate(), today) == 0 && myStrcmp(b->getStatus(), "paid") == 0)
            revenue += b->getAmount();
    }
    char rev[28]; myFtoa(revenue, rev);
    sfmlBufCat(out, cap, "Revenue today (paid): PKR "); sfmlBufCat(out, cap, rev);
    sfmlBufCat(out, cap, "\n\nPatients with unpaid totals:\n");
    for (int i = 0; i < patients.size(); i++) {
        Patient* p = patients.get(i);
        float owed = 0;
        for (int j = 0; j < bills.size(); j++) {
            Bill* b = bills.get(j);
            if (b->getPatientId() == p->getId() &&
                myStrcmp(b->getStatus(), "unpaid") == 0) owed += b->getAmount();
        }
        if (owed > 0.001f) {
            char ow[24]; myFtoa(owed, ow);
            sfmlBufCat(out, cap, p->getName()); sfmlBufCat(out, cap, " | PKR "); sfmlBufCat(out, cap, ow); sfmlBufCat(out, cap, "\n");
        }
    }
    sfmlBufCat(out, cap, "\nDoctor summary today (done | pend | no-show):\n");
    for (int i = 0; i < doctors.size(); i++) {
        Doctor* d = doctors.get(i);
        int dc = 0, dp = 0, dn = 0;
        for (int j = 0; j < appointments.size(); j++) {
            Appointment* a = appointments.get(j);
            if (a->getDoctorId() != d->getId()) continue;
            if (myStrcmp(a->getDate(), today) != 0) continue;
            if (myStrcmp(a->getStatus(), "completed") == 0) dc++;
            else if (myStrcmp(a->getStatus(), "pending") == 0) dp++;
            else if (myStrcmp(a->getStatus(), "noshow") == 0) dn++;
        }
        if (dc + dp + dn > 0) {
            sfmlBufCat(out, cap, d->getName()); sfmlBufCat(out, cap, " | ");
            char t[48]; myItoa(dc, t); sfmlBufCat(out, cap, t); sfmlBufCat(out, cap, " | ");
            myItoa(dp, t); sfmlBufCat(out, cap, t); sfmlBufCat(out, cap, " | ");
            myItoa(dn, t); sfmlBufCat(out, cap, t); sfmlBufCat(out, cap, "\n");
        }
    }
}

#ifdef _WIN32
static void sfmlShowSysConsole(bool visible) {
    HWND hc = ::GetConsoleWindow();
    if (hc) ::ShowWindow(hc, visible ? SW_SHOW : SW_HIDE);
}
#else
static void sfmlShowSysConsole(bool visible) {
    (void)visible;
}
#endif

static void sfmlTruncateForView(char* body, unsigned maxChars = 9800u) {
    int L = myStrlen(body);
    if ((unsigned)L <= maxChars) return;
    body[(int)maxChars] = '\0';
    myStrcat(body, "\n... [truncated: open .txt exports or use Full HD window] ...");
}

struct UiButton {
    sf::RectangleShape rect;
    sf::Text text;
    bool hovered = false;
    bool accentStyle = false;
    bool dangerStyle = false;

    void lay(const sf::Font& font, const char* label, sf::Vector2f pos, sf::Vector2f size,
             bool tealAccent, bool danger) {
        accentStyle = tealAccent;
        dangerStyle = danger;
        hovered = false;
        rect.setPosition(pos);
        rect.setSize(size);
        rect.setOutlineThickness(2.f);

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(19);
        if (accentStyle || dangerStyle) text.setFillColor(sf::Color::White);
        else text.setFillColor(UiTheme::TEXT_DARK);

        sf::FloatRect tb = text.getLocalBounds();
        text.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        text.setPosition(pos.x + size.x / 2.f, pos.y + size.y / 2.f);

        hoverRefresh(false);
    }

    void hoverRefresh(bool hz) {
        hovered = hz;
        if (dangerStyle) {
            rect.setOutlineColor(sf::Color(220, 150, 150));
            rect.setFillColor(hovered ? sf::Color(239, 83, 80) : sf::Color(211, 47, 47));
            text.setFillColor(sf::Color::White);
            return;
        }
        if (accentStyle) {
            rect.setOutlineColor(UiTheme::BORDER_SOFT);
            rect.setFillColor(hovered ? UiTheme::ACCENT_CYAN : UiTheme::HEADER_TEAL);
            text.setFillColor(sf::Color::White);
            return;
        }
        rect.setOutlineColor(UiTheme::BORDER_SOFT);
        rect.setFillColor(hovered ? sf::Color(227, 242, 251) : UiTheme::CARD_WHITE);
        text.setFillColor(UiTheme::TEXT_DARK);
    }

    bool contains(sf::Vector2f p) const { return rect.getGlobalBounds().contains(p); }

    void updateHover(sf::Vector2f mouse) { hoverRefresh(contains(mouse)); }

    void draw(sf::RenderWindow& w) const { w.draw(rect); w.draw(text); }
};

struct UiTextBox {
    sf::RectangleShape rect;
    sf::Text text;
    bool active = false;
    bool passwordMode = false;
    char value[64];

    void initClinical(const sf::Font& font, sf::Vector2f pos, sf::Vector2f size, bool isPassword) {
        passwordMode = isPassword;
        value[0] = '\0';

        rect.setPosition(pos);
        rect.setSize(size);
        rect.setFillColor(sf::Color(255, 255, 255));
        rect.setOutlineThickness(2.f);
        rect.setOutlineColor(UiTheme::BORDER_SOFT);

        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(UiTheme::TEXT_DARK);
        text.setPosition(pos.x + 12.f, pos.y + 12.f);
        text.setString("");
    }

    void clearBox() {
        value[0] = '\0';
        refreshText();
    }

    bool contains(sf::Vector2f p) const { return rect.getGlobalBounds().contains(p); }

    void setActive(bool a) {
        active = a;
        rect.setOutlineThickness(2.f);
        rect.setOutlineColor(active ? UiTheme::ACCENT_CYAN : UiTheme::BORDER_SOFT);
    }

    void refreshText() {
        if (!passwordMode) {
            text.setString(value);
            return;
        }
        // mask password
        char masked[64];
        int n = myStrlen(value);
        if (n > 60) n = 60;
        for (int i = 0; i < n; i++) masked[i] = '*';
        masked[n] = '\0';
        text.setString(masked);
    }

    void handleTextEntered(sf::Uint32 uni) {
        if (!active) return;
        // backspace
        int len = myStrlen(value);
        if (uni == 8) {
            if (len > 0) value[len - 1] = '\0';
            refreshText();
            return;
        }
        // enter ignored here
        if (uni == 13) return;
        // only visible ASCII
        if (uni < 32 || uni > 126) return;
        if (len >= 60) return;

        value[len] = (char)uni;
        value[len + 1] = '\0';
        refreshText();
    }
};

enum UiRole { ROLE_NONE = 0, ROLE_PATIENT = 1, ROLE_DOCTOR = 2, ROLE_ADMIN = 3 };

enum AdminSfPan { ASP_MENU = 0, ASP_ADD = 1, ASP_REMOVE = 2, ASP_DISCH = 3, ASP_VIEW = 4 };

/* returns false → caller should fall back to console menus */
bool runSfmlMediCoreShell(Storage<Patient>& patients, Storage<Doctor>& doctors,
    Storage<Appointment>& appointments, Storage<Bill>& bills,
    Storage<Prescription>& prescriptions, Admin* admin) {

    sf::Font font;
    if (!sfmlTryLoadFont(font)) {
        cout << "\n[SFML UI ERROR] Font load failed.\nPress Enter for console menus...";
        cin.get();
        return false;
    }

    sfmlShowSysConsole(false);

    sf::RenderWindow window(sf::VideoMode(1070u, 698u), "MediCore Hospital Portal");
    window.setFramerateLimit(90);

    int screen = 0; /* 0 = login , 1 = admin */
    AdminSfPan ap = ASP_MENU;

    UiRole selRole = ROLE_NONE;
    UiTextBox tbId; tbId.initClinical(font, {460.f, 322.f}, {530.f, 56.f}, false);
    UiTextBox tbPw; tbPw.initClinical(font, {460.f, 424.f}, {530.f, 56.f}, true);

    UiButton bPatient, bDoctor, bAdmin, bExit, bLogin;
    bPatient.lay(font, "Patient Portal", {42.f, 208.f}, {230.f, 58.f}, false, false);
    bDoctor.lay(font, "Doctor Portal", {295.f, 208.f}, {230.f, 58.f}, false, false);
    bAdmin.lay(font, "Administration", {548.f, 208.f}, {230.f, 58.f}, false, false);
    bExit.lay(font, "Exit Application", {802.f, 208.f}, {226.f, 58.f}, false, true);
    bLogin.lay(font, "Secure login", {460.f, 518.f}, {240.f, 58.f}, true, false);

    sf::RectangleShape banner;
    banner.setFillColor(UiTheme::HEADER_TEAL);
    banner.setSize(sf::Vector2f(1070.f, 86.f));

    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::White);
    title.setString("MediCore Hospital Management");
    title.setPosition(46.f, 18.f);

    sf::Text sub;
    sub.setFont(font);
    sub.setCharacterSize(17);
    sub.setFillColor(sf::Color(210, 232, 234));
    sub.setString("Secure access to patient care, clinician tools, and hospital administration.");
    sub.setPosition(46.f, 56.f);

    sf::Text cardHint;
    cardHint.setFont(font);
    cardHint.setCharacterSize(16);
    cardHint.setFillColor(UiTheme::TEXT_SOFT);
    cardHint.setString("STEP 1  Choose portal   |   STEP 2  Credentials   |   STEP 3  Login");
    cardHint.setPosition(46.f, 124.f);

    sf::Text lblId;
    lblId.setFont(font); lblId.setCharacterSize(18); lblId.setFillColor(UiTheme::TEXT_DARK);
    lblId.setString("User ID"); lblId.setPosition(460.f, 288.f);

    sf::Text lblPw;
    lblPw.setFont(font); lblPw.setCharacterSize(18); lblPw.setFillColor(UiTheme::TEXT_DARK);
    lblPw.setString("Password"); lblPw.setPosition(460.f, 390.f);

    sf::RectangleShape loginCard;
    loginCard.setFillColor(UiTheme::CARD_WHITE);
    loginCard.setOutlineColor(UiTheme::BORDER_SOFT);
    loginCard.setOutlineThickness(2.f);
    loginCard.setSize(sf::Vector2f(986.f, 412.f));
    loginCard.setPosition(42.f, 182.f);

    sf::Text loginErr;
    loginErr.setFont(font); loginErr.setCharacterSize(17);
    loginErr.setFillColor(UiTheme::ERROR_ROSE);
    loginErr.setPosition(716.f, 528.f); loginErr.setString("");

    auto roleRefresh = [&](void) {
        bPatient.lay(font, "Patient Portal", {42.f, 208.f}, {230.f, 58.f}, selRole == ROLE_PATIENT, false);
        bDoctor.lay(font, "Doctor Portal", {295.f, 208.f}, {230.f, 58.f}, selRole == ROLE_DOCTOR, false);
        bAdmin.lay(font, "Administration", {548.f, 208.f}, {230.f, 58.f}, selRole == ROLE_ADMIN, false);
    };

    /* --- Admin dashboard --- */
    UiButton ad_m1, ad_m2, ad_m3, ad_m4, ad_m5, ad_m6, ad_m7, ad_m8, ad_m9, ad_back;
    float ax0 = 52.f, ax1 = 540.f, bw = 468.f, bh = 60.f;

    auto layoutAdminBtns = [&]() {
        ad_m1.lay(font, "1  Register new doctor", {ax0, 132.f}, {bw, bh}, false, false);
        ad_m2.lay(font, "2  Remove doctor",       {ax1, 132.f}, {bw, bh}, false, false);
        ad_m3.lay(font, "3  All patients snapshot", {ax0, 210.f}, {bw, bh}, false, false);
        ad_m4.lay(font, "4  Doctor directory",     {ax1, 210.f}, {bw, bh}, false, false);
        ad_m5.lay(font, "5  Appointments ledger", {ax0, 288.f}, {bw, bh}, false, false);
        ad_m6.lay(font, "6  Unpaid bills",        {ax1, 288.f}, {bw, bh}, false, false);
        ad_m7.lay(font, "7  Discharge patient",   {ax0, 366.f}, {bw, bh}, false, false);
        ad_m8.lay(font, "8  Security events",      {ax1, 366.f}, {bw, bh}, false, false);
        ad_m9.lay(font, "9  Daily operations report", {ax0, 444.f}, {bw, bh}, false, false);
        ad_back.lay(font, "<< Back to portal login", {ax1, 444.f}, {bw, bh}, false, true);
    };
    layoutAdminBtns();

    sf::Text adHead;
    adHead.setFont(font); adHead.setCharacterSize(26); adHead.setFillColor(sf::Color::White);
    adHead.setPosition(52.f, 24.f); adHead.setString("Administrative Dashboard");

    sf::Text adSub;
    adSub.setFont(font); adSub.setCharacterSize(16); adSub.setFillColor(sf::Color(200, 230, 228));
    adSub.setPosition(52.f, 66.f); adSub.setString("All changes write to your existing MediCore text databases.");

    sf::Text adminToast;
    adminToast.setFont(font); adminToast.setCharacterSize(17); adminToast.setFillColor(UiTheme::TEXT_DARK);
    adminToast.setPosition(52.f, 606.f); adminToast.setString("");

    sf::Text formTitle;
    formTitle.setFont(font); formTitle.setCharacterSize(20); formTitle.setFillColor(UiTheme::TEXT_DARK);
    formTitle.setPosition(52.f, 118.f);

    UiTextBox workTb; workTb.initClinical(font, {52.f, 192.f}, {640.f, 56.f}, false);
    UiButton bNext, bBackF, bCancelF;
    bNext.lay(font, "Continue", {720.f, 192.f}, {150.f, 56.f}, true, false);
    bBackF.lay(font, "Back", {890.f, 192.f}, {120.f, 56.f}, false, false);
    bCancelF.lay(font, "Cancel", {1020.f, 192.f}, {0.f, 0.f}, false, true); /* resized below */
    bCancelF.lay(font, "Cancel", {720.f, 268.f}, {290.f, 48.f}, false, true);

    char ad_name[60], ad_spec[60], ad_contact[15], ad_pw[50], ad_fee[24];
    ad_name[0] = ad_spec[0] = ad_contact[0] = ad_pw[0] = ad_fee[0] = '\0';
    int addStep = 0;

    char viewBody[26000];
    viewBody[0] = '\0';
    float viewScroll = 0.f;

    sf::Text viewTxt;
    viewTxt.setFont(font);
    viewTxt.setCharacterSize(15);
    viewTxt.setFillColor(UiTheme::TEXT_DARK);
    viewTxt.setPosition(56.f, 124.f);

    sf::Text viewHint;
    viewHint.setFont(font);
    viewHint.setCharacterSize(13);
    viewHint.setFillColor(UiTheme::TEXT_SOFT);
    viewHint.setString("(Use Arrow Up / Down to scroll; Esc exits)");
    viewHint.setPosition(52.f, 92.f);

    UiButton bViewBack;
    bViewBack.lay(font, "Return to dashboard", {52.f, 612.f}, {280.f, 52.f}, true, false);

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();

            if (screen == 0) {
                if (ev.type == sf::Event::TextEntered) {
                    tbId.handleTextEntered(ev.text.unicode);
                    tbPw.handleTextEntered(ev.text.unicode);
                }
                if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Tab) {
                    if (tbId.active) { tbId.setActive(false); tbPw.setActive(true); }
                    else if (tbPw.active) { tbPw.setActive(false); tbId.setActive(true); }
                    else { tbId.setActive(true); tbPw.setActive(false); }
                }
            } else if (screen == 1) {
                if (ap == ASP_ADD || ap == ASP_REMOVE || ap == ASP_DISCH) {
                    if (ev.type == sf::Event::TextEntered)
                        workTb.handleTextEntered(ev.text.unicode);
                }
                if (ap == ASP_VIEW) {
                    if (ev.type == sf::Event::KeyPressed) {
                        if (ev.key.code == sf::Keyboard::Up) viewScroll += 26.f;
                        if (ev.key.code == sf::Keyboard::Down) viewScroll -= 26.f;
                        if (ev.key.code == sf::Keyboard::Escape) { ap = ASP_MENU; viewScroll = 0.f; }
                    }
                }
            }

            if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mouse((float)ev.mouseButton.x, (float)ev.mouseButton.y);

                if (screen == 0) {
                    if (bExit.contains(mouse)) { window.close(); break; }
                    if (bPatient.contains(mouse)) { selRole = ROLE_PATIENT; roleRefresh(); loginErr.setString(""); }
                    if (bDoctor.contains(mouse))  { selRole = ROLE_DOCTOR;  roleRefresh(); loginErr.setString(""); }
                    if (bAdmin.contains(mouse))   { selRole = ROLE_ADMIN;   roleRefresh(); loginErr.setString(""); }
                    tbId.setActive(tbId.contains(mouse));
                    tbPw.setActive(tbPw.contains(mouse));

                    if (bLogin.contains(mouse)) {
                        loginErr.setString("");
                        if (selRole == ROLE_NONE) { loginErr.setString("Select a portal first."); continue; }
                        if (!Validator::isAllDigits(tbId.value)) { loginErr.setString("ID must be numeric."); continue; }
                        if (myStrlen(tbPw.value) == 0) { loginErr.setString("Password required."); continue; }

                        int uid = myAtoi(tbId.value);

                        if (selRole == ROLE_ADMIN) {
                            if (!loginAdminWithCredentials(admin, uid, tbPw.value)) {
                                loginErr.setString("Invalid admin credentials.");
                                continue;
                            }
                            screen = 1;
                            ap = ASP_MENU;
                            adminToast.setString("");
                            window.setTitle("MediCore - Administration");
                            continue;
                        }
                        if (selRole == ROLE_PATIENT) {
                            Patient* pat = loginPatientWithCredentials(patients, uid, tbPw.value);
                            if (!pat) { loginErr.setString("Patient login failed."); continue; }
                            sfmlShowSysConsole(true);
                            window.setVisible(false);
                            runPatientMenu(pat, doctors, appointments, bills, prescriptions);
                            window.setVisible(true);
                            sfmlShowSysConsole(false);
                            tbPw.clearBox(); tbId.clearBox();
                            continue;
                        }
                        if (selRole == ROLE_DOCTOR) {
                            Doctor* doc = loginDoctorWithCredentials(doctors, uid, tbPw.value);
                            if (!doc) { loginErr.setString("Doctor login failed."); continue; }
                            sfmlShowSysConsole(true);
                            window.setVisible(false);
                            while (true) {
                                cout << "\nWelcome, Dr. " << doc->getName()
                                     << " | Specialization: " << doc->getSpec()
                                     << "\n===============================================\n"
                                     << "1. View Today's Appointments\n"
                                     << "2. Mark Appointment Complete\n"
                                     << "3. Mark Appointment No-Show\n"
                                     << "4. Write Prescription\n"
                                     << "5. View Patient Medical History\n"
                                     << "6. Logout\n"
                                     << "Choice: ";
                                int ch = readInt();
                                switch (ch) {
                                    case 1: doctorViewTodayAppointments(doc, patients, appointments); break;
                                    case 2: doctorMarkComplete(doc, appointments); break;
                                    case 3: doctorMarkNoShow(doc, appointments); break;
                                    case 4: doctorWritePrescription(doc, appointments, prescriptions); break;
                                    case 5: doctorViewPatientHistory(doc, patients, appointments, prescriptions); break;
                                    case 6: cout << "Logged out.\n"; break;
                                    default: cout << "Invalid choice.\n"; continue;
                                }
                                if (ch == 6) break;
                            }
                            window.setVisible(true);
                            sfmlShowSysConsole(false);
                            tbPw.clearBox(); tbId.clearBox();
                            continue;
                        }
                    }
                } else if (screen == 1) {
                    if (ap == ASP_MENU) {
                        if (ad_back.contains(mouse)) {
                            screen = 0;
                            window.setTitle("MediCore Hospital Portal");
                            tbPw.clearBox(); tbId.clearBox();
                            loginErr.setString("");
                            continue;
                        }
                        if (ad_m1.contains(mouse)) {
                            ap = ASP_ADD; addStep = 0;
                            ad_name[0] = ad_spec[0] = ad_contact[0] = ad_pw[0] = ad_fee[0] = '\0';
                            workTb.clearBox();
                            formTitle.setString("New physician — full name (max 50 characters)");
                        } else if (ad_m2.contains(mouse)) {
                            ap = ASP_REMOVE; workTb.clearBox();
                            formTitle.setString("Remove doctor — enter numeric doctor ID");
                        } else if (ad_m3.contains(mouse)) {
                            sfmlBuildPatientsTxt(patients, bills, viewBody, (int)sizeof(viewBody));
                            sfmlTruncateForView(viewBody);
                            viewTxt.setString(viewBody);
                            ap = ASP_VIEW; viewScroll = 0.f;
                            formTitle.setString("Read-only: patients register");
                        } else if (ad_m4.contains(mouse)) {
                            sfmlBuildDoctorsTxt(doctors, viewBody, (int)sizeof(viewBody));
                            sfmlTruncateForView(viewBody);
                            viewTxt.setString(viewBody);
                            ap = ASP_VIEW; viewScroll = 0.f;
                            formTitle.setString("Read-only: clinical staff directory");
                        } else if (ad_m5.contains(mouse)) {
                            sfmlBuildAppointmentsTxt(patients, doctors, appointments, viewBody, (int)sizeof(viewBody));
                            sfmlTruncateForView(viewBody);
                            viewTxt.setString(viewBody);
                            ap = ASP_VIEW; viewScroll = 0.f;
                            formTitle.setString("Read-only: appointments ledger");
                        } else if (ad_m6.contains(mouse)) {
                            sfmlBuildUnpaidBillsTxt(patients, bills, viewBody, (int)sizeof(viewBody));
                            sfmlTruncateForView(viewBody);
                            viewTxt.setString(viewBody);
                            ap = ASP_VIEW; viewScroll = 0.f;
                            formTitle.setString("Outstanding accounts receivable");
                        } else if (ad_m7.contains(mouse)) {
                            ap = ASP_DISCH; workTb.clearBox();
                            formTitle.setString("Discharge — patient ID (all bills must be settled)");
                        } else if (ad_m8.contains(mouse)) {
                            sfmlBuildSecurityLogTxt(viewBody, (int)sizeof(viewBody));
                            sfmlTruncateForView(viewBody);
                            viewTxt.setString(viewBody);
                            ap = ASP_VIEW; viewScroll = 0.f;
                            formTitle.setString("Information security audit trail");
                        } else if (ad_m9.contains(mouse)) {
                            sfmlBuildDailyReportTxt(patients, doctors, appointments, bills, viewBody,
                                (int)sizeof(viewBody));
                            sfmlTruncateForView(viewBody);
                            viewTxt.setString(viewBody);
                            ap = ASP_VIEW; viewScroll = 0.f;
                            formTitle.setString("End-of-shift operational summary");
                        }
                    } else if (ap == ASP_VIEW && bViewBack.contains(mouse)) {
                        ap = ASP_MENU;
                        viewScroll = 0.f;
                    } else if (ap == ASP_ADD || ap == ASP_REMOVE || ap == ASP_DISCH) {
                        tbId.setActive(false); tbPw.setActive(false);
                        workTb.setActive(workTb.contains(mouse));
                        bool hitNext = bNext.contains(mouse);
                        bool hitBack = bBackF.contains(mouse);
                        bool hitCan = bCancelF.contains(mouse);
                        if (hitCan) { ap = ASP_MENU; adminToast.setString("Action cancelled."); continue; }

                        if (ap == ASP_ADD) {
                            if (hitBack) {
                                if (addStep > 0) { addStep--; workTb.clearBox();
                                    const char* labels[] = {
                                        "New physician — full name (max 50 characters)",
                                        "Medical specialty",
                                        "Contact number (exactly 11 digits)",
                                        "Account password (min 6 characters)",
                                        "Consultation fee (positive PKR amount)"
                                    };
                                    if (addStep == 0) { myStrcpy(workTb.value, ad_name); workTb.refreshText(); }
                                    if (addStep == 1) { myStrcpy(workTb.value, ad_spec); workTb.refreshText(); }
                                    if (addStep == 2) { myStrcpy(workTb.value, ad_contact); workTb.refreshText(); }
                                    if (addStep == 3) { myStrcpy(workTb.value, ad_pw); workTb.refreshText(); }
                                    if (addStep == 4) { myStrcpy(workTb.value, ad_fee); workTb.refreshText(); }
                                    formTitle.setString(labels[addStep]);
                                }
                                continue;
                            }
                            if (hitNext) {
                                if (addStep == 0) {
                                    if (myStrlen(workTb.value) < 1) { adminToast.setString("Name required."); continue; }
                                    myStrncpy(ad_name, workTb.value, 60);
                                    addStep = 1; workTb.clearBox(); formTitle.setString("Medical specialty");
                                } else if (addStep == 1) {
                                    if (myStrlen(workTb.value) < 1) { adminToast.setString("Specialization required."); continue; }
                                    myStrncpy(ad_spec, workTb.value, 60);
                                    addStep = 2; workTb.clearBox(); formTitle.setString("Contact number (exactly 11 digits)");
                                } else if (addStep == 2) {
                                    if (!Validator::isValidContact(workTb.value)) {
                                        adminToast.setString("Invalid contact — eleven digits."); continue;
                                    }
                                    myStrncpy(ad_contact, workTb.value, 15);
                                    addStep = 3; workTb.clearBox(); formTitle.setString("Password (minimum 6 characters)");
                                } else if (addStep == 3) {
                                    if (!Validator::isValidPassword(workTb.value)) {
                                        adminToast.setString("Weak password."); continue;
                                    }
                                    myStrncpy(ad_pw, workTb.value, 50);
                                    addStep = 4; workTb.clearBox(); formTitle.setString("Consultation fee amount (PKR)");
                                } else {
                                    myStrncpy(ad_fee, workTb.value, sizeof(ad_fee)-1); ad_fee[sizeof(ad_fee)-1]=0;
                                    if (!Validator::isPositiveFloat(ad_fee)) {
                                        adminToast.setString("Fee must be a positive figure."); continue;
                                    }
                                    float fee = myAtof(ad_fee);
                                    adminAddDoctorCore(doctors, ad_name, ad_spec, ad_contact, ad_pw, fee);
                                    char tmsg[140]; char ib[16];
                                    myItoa(FileHandler::getMaxId("doctors.txt"), ib);
                                    tmsg[0] = '\0'; myStrcat(tmsg, "Doctor registered successfully. Assigned ID "); myStrcat(tmsg, ib);
                                    adminToast.setString(tmsg);
                                    ap = ASP_MENU;
                                }
                                continue;
                            }
                        }

                        if (ap == ASP_REMOVE) {
                            if (hitNext) {
                                if (!Validator::isAllDigits(workTb.value)) { adminToast.setString("Doctor ID numeric only."); continue; }
                                char er[160];
                                if (!adminRemoveDoctorById(doctors, appointments, myAtoi(workTb.value), er, 160))
                                    adminToast.setString(er);
                                else adminToast.setString("Doctor record removed permanently.");
                                ap = ASP_MENU;
                            }
                        }
                        if (ap == ASP_DISCH) {
                            if (hitNext) {
                                if (!Validator::isAllDigits(workTb.value)) { adminToast.setString("Patient ID numeric only."); continue; }
                                char er[160];
                                if (!adminDischargePatientById(patients, doctors, appointments, bills, prescriptions,
                                    myAtoi(workTb.value), er, 160))
                                    adminToast.setString(er);
                                else adminToast.setString("Patient discharged — records archived.");
                                ap = ASP_MENU;
                            }
                        }
                    }
                }
            }
        }

        sf::Vector2f mouseMv((float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y);
        if (screen == 0) {
            bPatient.updateHover(mouseMv); bDoctor.updateHover(mouseMv); bAdmin.updateHover(mouseMv);
            bExit.updateHover(mouseMv); bLogin.updateHover(mouseMv);
        } else if (screen == 1) {
            if (ap == ASP_MENU) {
                ad_m1.updateHover(mouseMv); ad_m2.updateHover(mouseMv); ad_m3.updateHover(mouseMv);
                ad_m4.updateHover(mouseMv); ad_m5.updateHover(mouseMv); ad_m6.updateHover(mouseMv);
                ad_m7.updateHover(mouseMv); ad_m8.updateHover(mouseMv); ad_m9.updateHover(mouseMv);
                ad_back.updateHover(mouseMv);
            } else if (ap == ASP_VIEW) {
                bViewBack.updateHover(mouseMv);
            } else {
                bNext.updateHover(mouseMv); bBackF.updateHover(mouseMv); bCancelF.updateHover(mouseMv);
            }
        }

        window.clear(UiTheme::BG_CLOUD);

        sf::RectangleShape divider;
        divider.setFillColor(sf::Color(210, 224, 234));
        divider.setPosition(418.f, 182.f); divider.setSize(sf::Vector2f(2.f, 408.f));

        if (screen == 0) {
            window.draw(banner); window.draw(title); window.draw(sub);
            window.draw(loginCard); window.draw(cardHint); window.draw(divider);
            lblId.setString("Staff / patient ID"); window.draw(lblId);
            lblPw.setString("Password"); window.draw(lblPw);
            window.draw(tbId.rect); window.draw(tbPw.rect);
            window.draw(tbId.text); window.draw(tbPw.text);
            bPatient.draw(window); bDoctor.draw(window); bAdmin.draw(window);
            bExit.draw(window); bLogin.draw(window);
            window.draw(loginErr);
        } else {
            window.draw(banner); window.draw(adHead); window.draw(adSub); window.draw(adminToast);
            if (ap == ASP_MENU) {
                ad_m1.draw(window); ad_m2.draw(window); ad_m3.draw(window);
                ad_m4.draw(window); ad_m5.draw(window); ad_m6.draw(window);
                ad_m7.draw(window); ad_m8.draw(window); ad_m9.draw(window);
                ad_back.draw(window);
            } else if (ap == ASP_VIEW) {
                window.draw(formTitle);
                window.draw(viewHint);
                if (viewScroll > 0.f) viewScroll = 0.f;
                if (viewScroll < -6200.f) viewScroll = -6200.f;
                viewTxt.setPosition(52.f, 130.f + viewScroll);
                window.draw(viewTxt);
                bViewBack.draw(window);
            } else {
                window.draw(formTitle);
                window.draw(workTb.rect); window.draw(workTb.text);
                bNext.draw(window); bBackF.draw(window); bCancelF.draw(window);
                workTb.passwordMode = (ap == ASP_ADD && addStep == 3);
                workTb.refreshText();
            }
        }

        window.display();
    }

    sfmlShowSysConsole(true);
    return true;
}

#endif

// ============================================================
//  SECTION 11 : Startup — ensure files + seed defaults
// ============================================================

void ensureDefaultFiles() {
    FileHandler::ensureFile("patients.txt");
    FileHandler::ensureFile("doctors.txt");
    FileHandler::ensureFile("appointments.txt");
    FileHandler::ensureFile("bills.txt");
    FileHandler::ensureFile("prescriptions.txt");
    FileHandler::ensureFile("security_log.txt");
    FileHandler::ensureFile("discharged.txt");

    // Seed admin.txt with default admin if empty
    FileHandler::ensureFile("admin.txt");
    ifstream adminCheck("admin.txt");
    char testLine[50]; bool hasAdmin = false;
    if (adminCheck.getline(testLine, 50) && myStrlen(testLine) > 0) hasAdmin = true;
    adminCheck.close();
    if (!hasAdmin) {
        // Default admin: id=1, name=Admin, password=admin123
        FileHandler::appendLine("admin.txt", "1,Admin,admin123");
    }
}

// ============================================================
//  SECTION 12 : Memory cleanup helpers
// ============================================================

void freeStorage(Storage<Patient>& patients,
                 Storage<Doctor>& doctors,
                 Storage<Appointment>& appointments,
                 Storage<Bill>& bills,
                 Storage<Prescription>& prescriptions,
                 Admin* admin) {
    for (int i = 0; i < patients.size(); i++)     delete patients.get(i);
    for (int i = 0; i < doctors.size(); i++)       delete doctors.get(i);
    for (int i = 0; i < appointments.size(); i++)  delete appointments.get(i);
    for (int i = 0; i < bills.size(); i++)         delete bills.get(i);
    for (int i = 0; i < prescriptions.size(); i++) delete prescriptions.get(i);
    delete admin;
}

// ============================================================
//  SECTION 13 : main()
// ============================================================

int main() {
    // Ensure all required files exist
    ensureDefaultFiles();

    // Load all data into in-memory storage
    Storage<Patient>     patients;
    Storage<Doctor>      doctors;
    Storage<Appointment> appointments;
    Storage<Bill>        bills;
    Storage<Prescription>prescriptions;

    FileHandler::loadPatients(patients);
    FileHandler::loadDoctors(doctors);
    FileHandler::loadAppointments(appointments);
    FileHandler::loadBills(bills);
    FileHandler::loadPrescriptions(prescriptions);

    Admin* admin = FileHandler::loadAdmin();
    if (!admin) {
        cout << "CRITICAL: admin.txt missing or empty. Exiting.\n";
        return 1;
    }

#ifdef USE_SFML_UI
    if (!runSfmlMediCoreShell(patients, doctors, appointments, bills, prescriptions, admin))
        cout << "SFML UI not available — falling back to console menus.\n";
    else {
        freeStorage(patients, doctors, appointments, bills, prescriptions, admin);
        return 0;
    }
#endif

    // Main loop
    while (true) {
        cout << "\nWelcome to MediCore Hospital Management System\n"
             << "===============================================\n"
             << "Login as:\n"
             << "1. Patient\n"
             << "2. Doctor\n"
             << "3. Admin\n"
             << "4. Exit\n"
             << "Choice: ";
        int choice = readInt();

        switch (choice) {
            case 1:
                runPatientSession(patients, doctors, appointments, bills, prescriptions);
                break;
            case 2:
                runDoctorSession(doctors, patients, appointments, bills, prescriptions);
                break;
            case 3:
                runAdminSession(admin, patients, doctors, appointments, bills, prescriptions);
                break;
            case 4:
                cout << "Goodbye!\n";
                freeStorage(patients, doctors, appointments, bills, prescriptions, admin);
                return 0;
            default:
                cout << "Invalid choice.\n";
        }
    }
}
