/*
Project Title: Hospital Patient Registration & Follow-up Tracker
Student Name: NISHANK CHHIPA
Register No: 25BCE1642
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATIENTS 100
#define MAX_VISITS 500

// 3.1 Structure Definition
struct Patient {
    int patientID;
    char name[50];
    int age;
    char phone[15];
};

struct Visit {
    int patientID;
    char date[15];
    char diagnosis[100];
    char prescription[100];
};

// Global arrays to store data in memory
struct Patient patients[MAX_PATIENTS];
int patientCount = 0;

struct Visit visits[MAX_VISITS];
int visitCount = 0;

// Function Prototypes
void loadData();
void saveData();
void registerPatient();
void addVisitRecord();
void searchPatient();
void showVisitHistory();
void showFrequentVisitors();
int findPatientIndex(int id);

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

    int choice;
    
    // Load data from files at startup
    loadData();

    // Menu-driven program using switch/case
    do {
        printf("\n=== Hospital Patient Registration & Follow-up Tracker ===\n");
        printf("1. Register Patient\n");
        printf("2. Add Visit Record\n");
        printf("3. Search Patient by ID\n");
        printf("4. Show Visit History for Patient\n");
        printf("5. Show Frequent Visitors (Visit Count)\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        fflush(stdout);
        
        // Input validation for menu
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while(getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1:
                registerPatient();
                break;
            case 2:
                addVisitRecord();
                break;
            case 3:
                searchPatient();
                break;
            case 4:
                showVisitHistory();
                break;
            case 5:
                showFrequentVisitors();
                break;
            case 6:
                printf("Exiting program. Data is saved.\n");
                break;
            default:
                printf("Invalid choice! Please select between 1 and 6.\n");
        }
    } while (choice != 6);

    return 0;
}

// Helper function to find if a patient exists
int findPatientIndex(int id) {
    for (int i = 0; i < patientCount; i++) {
        if (patients[i].patientID == id) {
            return i;
        }
    }
    return -1; // Not found
}

// 1. Register patient: (PatientID, Name, Age, Phone)
void registerPatient() {
    if (patientCount >= MAX_PATIENTS) {
        printf("Error: Maximum patient limit reached.\n");
        return;
    }

    struct Patient p;
    printf("\n--- Register New Patient ---\n");
    printf("Enter Patient ID: ");
    scanf("%d", &p.patientID);

    // Input Validation: Check for duplicate ID
    if (findPatientIndex(p.patientID) != -1) {
        printf("Error: Patient ID %d already exists!\n", p.patientID);
        return;
    }

    printf("Enter Name: ");
    scanf(" %[^\n]s", p.name); // Read string with spaces
    
    printf("Enter Age: ");
    scanf("%d", &p.age);
    // Input Validation: Range check for age
    if (p.age <= 0 || p.age > 120) {
        printf("Error: Invalid age entered.\n");
        return;
    }

    printf("Enter Phone: ");
    scanf(" %[^\n]s", p.phone);

    patients[patientCount] = p;
    patientCount++;
    saveData(); // Save state after modification
    printf("Patient successfully registered!\n");
}

// 2. Add visit record: (PatientID, Date, Diagnosis, Prescription note)
void addVisitRecord() {
    if (visitCount >= MAX_VISITS) {
        printf("Error: Maximum visit limit reached.\n");
        return;
    }

    struct Visit v;
    printf("\n--- Add Visit Record ---\n");
    printf("Enter Patient ID: ");
    scanf("%d", &v.patientID);

    // Validate that the patient exists before adding a visit
    if (findPatientIndex(v.patientID) == -1) {
        printf("Error: Patient ID %d not found. Please register the patient first.\n", v.patientID);
        return;
    }

    printf("Enter Date (DD/MM/YYYY): ");
    scanf(" %[^\n]s", v.date);
    
    printf("Enter Diagnosis: ");
    scanf(" %[^\n]s", v.diagnosis);
    
    printf("Enter Prescription Note: ");
    scanf(" %[^\n]s", v.prescription);

    visits[visitCount] = v;
    visitCount++;
    saveData(); // Save state after modification
    printf("Visit record successfully added!\n");
}

// 3. Search patient by ID
void searchPatient() {
    int id;
    printf("\n--- Search Patient ---\n");
    printf("Enter Patient ID to search: ");
    scanf("%d", &id);

    int index = findPatientIndex(id);
    if (index != -1) {
        printf("\nPatient Found:\n");
        printf("ID: %d | Name: %s | Age: %d | Phone: %s\n", 
               patients[index].patientID, patients[index].name, 
               patients[index].age, patients[index].phone);
    } else {
        printf("Patient with ID %d not found.\n", id);
    }
}

// 4. Show visit history for patient
void showVisitHistory() {
    int id;
    printf("\n--- Visit History ---\n");
    printf("Enter Patient ID: ");
    scanf("%d", &id);

    if (findPatientIndex(id) == -1) {
        printf("Error: Patient ID %d not found.\n", id);
        return;
    }

    int foundVisits = 0;
    printf("\nVisit History for Patient ID %d:\n", id);
    printf("--------------------------------------------------\n");
    for (int i = 0; i < visitCount; i++) {
        if (visits[i].patientID == id) {
            printf("Date: %s\nDiagnosis: %s\nPrescription: %s\n", 
                   visits[i].date, visits[i].diagnosis, visits[i].prescription);
            printf("--------------------------------------------------\n");
            foundVisits++;
        }
    }

    if (foundVisits == 0) {
        printf("No visits recorded for this patient.\n");
    }
}

// 5. Show count of visits per patient and identify "frequent visitors"
void showFrequentVisitors() {
    printf("\n--- Frequent Visitors Report ---\n");
    printf("Patients with 3 or more visits are considered 'Frequent'.\n\n");
    
    for (int i = 0; i < patientCount; i++) {
        int vCount = 0;
        for (int j = 0; j < visitCount; j++) {
            if (visits[j].patientID == patients[i].patientID) {
                vCount++;
            }
        }
        
        printf("Patient ID: %d | Name: %s | Total Visits: %d", 
               patients[i].patientID, patients[i].name, vCount);
               
        if (vCount >= 3) { // Threshold for frequent visitor
            printf(" *** FREQUENT VISITOR ***");
        }
        printf("\n");
    }
}

// File Handling: Load Data
void loadData() {
    FILE *pFile = fopen("patients.txt", "r");
    if (pFile != NULL) {
        while (fscanf(pFile, "%d,%49[^,],%d,%14[^\n]\n", 
               &patients[patientCount].patientID, 
               patients[patientCount].name, 
               &patients[patientCount].age, 
               patients[patientCount].phone) == 4) {
            patientCount++;
        }
        fclose(pFile);
    }

    FILE *vFile = fopen("visits.txt", "r");
    if (vFile != NULL) {
        while (fscanf(vFile, "%d,%14[^,],%99[^,],%99[^\n]\n", 
               &visits[visitCount].patientID, 
               visits[visitCount].date, 
               visits[visitCount].diagnosis, 
               visits[visitCount].prescription) == 4) {
            visitCount++;
        }
        fclose(vFile);
    }
}

// File Handling: Save Data
void saveData() {
    FILE *pFile = fopen("patients.txt", "w");
    if (pFile != NULL) {
        for (int i = 0; i < patientCount; i++) {
            fprintf(pFile, "%d,%s,%d,%s\n", 
                    patients[i].patientID, patients[i].name, 
                    patients[i].age, patients[i].phone);
        }
        fclose(pFile);
    }

    FILE *vFile = fopen("visits.txt", "w");
    if (vFile != NULL) {
        for (int i = 0; i < visitCount; i++) {
            fprintf(vFile, "%d,%s,%s,%s\n", 
                    visits[i].patientID, visits[i].date, 
                    visits[i].diagnosis, visits[i].prescription);
        }
        fclose(vFile);
    }
}