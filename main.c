#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
// This tells WebAssembly to pause C execution and wait for a JavaScript Promise
EM_ASYNC_JS(void, get_input_from_js, (char* buffer, int max_size), {
    const input = await window.waitForUserInput();
    stringToUTF8(input, buffer, max_size);
});

void get_input(char* buffer, int max_size) {
    get_input_from_js(buffer, max_size);
}
#else
// Fallback for standard desktop GCC compilation
void get_input(char* buffer, int max_size) {
    if (fgets(buffer, max_size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
    }
}
#endif

#define MAX_PATIENTS 100
#define MAX_VISITS 500

struct Patient {
    int id; char name[50]; int age; char phone[15]; int visit_count;
};
struct Visit {
    int patient_id; char date[20]; char diagnosis[100]; char prescription[100];
};

struct Patient patients[MAX_PATIENTS];
struct Visit visits[MAX_VISITS];
int p_count = 0, v_count = 0;

int findPatientIndex(int id) {
    for (int i = 0; i < p_count; i++) {
        if (patients[i].id == id) return i;
    }
    return -1;
}

void registerPatientInteractive() {
    if (p_count >= MAX_PATIENTS) {
        printf("[-] Error: Maximum limit reached.\n"); return;
    }
    char buffer[256];
    struct Patient p;

    printf("\n--- Register New Patient ---\n");
    printf("Enter Patient ID:\n"); 
    get_input(buffer, sizeof(buffer)); 
    p.id = atoi(buffer);
    
    if (findPatientIndex(p.id) != -1) {
        printf("[-] Error: Patient ID %d already exists!\n", p.id); return;
    }

    printf("Enter Full Name:\n"); 
    get_input(p.name, sizeof(p.name));

    printf("Enter Age:\n"); 
    get_input(buffer, sizeof(buffer)); 
    p.age = atoi(buffer);
    if(p.age <= 0 || p.age > 120) {
        printf("[-] Invalid Age! Registration failed.\n"); return;
    }

    printf("Enter Phone:\n"); 
    get_input(p.phone, sizeof(p.phone));

    p.visit_count = 0;
    patients[p_count++] = p;
    printf("[+] Patient '%s' registered (ID: %d).\n", p.name, p.id);
}

void addVisitInteractive() {
    char buffer[256];
    struct Visit v;

    printf("\n--- Add Visit Record ---\n");
    printf("Enter Patient ID:\n"); 
    get_input(buffer, sizeof(buffer)); 
    v.patient_id = atoi(buffer);

    int idx = findPatientIndex(v.patient_id);
    if (idx == -1) {
        printf("[-] Error: Patient ID %d not found.\n", v.patient_id); return;
    }

    printf("Enter Date (DD-MM-YYYY):\n"); 
    get_input(v.date, sizeof(v.date));

    printf("Enter Diagnosis:\n"); 
    get_input(v.diagnosis, sizeof(v.diagnosis));

    printf("Enter Prescription:\n"); 
    get_input(v.prescription, sizeof(v.prescription));

    visits[v_count++] = v;
    patients[idx].visit_count++;
    printf("[+] Visit recorded for ID %d.\n", v.patient_id);
}

void searchPatientInteractive() {
    char buffer[256];
    printf("\n--- Search Patient ---\n");
    printf("Enter Patient ID:\n"); 
    get_input(buffer, sizeof(buffer)); 
    int id = atoi(buffer);

    int idx = findPatientIndex(id);
    if (idx != -1) {
        printf("\n ID     : %d\n Name   : %s\n Age    : %d\n Phone  : %s\n Visits : %d\n", 
               patients[idx].id, patients[idx].name, patients[idx].age, patients[idx].phone, patients[idx].visit_count);
    } else {
        printf("[-] Patient not found.\n");
    }
}

void showHistoryInteractive() {
    char buffer[256];
    printf("\n--- Visit History ---\n");
    printf("Enter Patient ID:\n"); 
    get_input(buffer, sizeof(buffer)); 
    int id = atoi(buffer);

    int found = 0;
    for (int i = 0; i < v_count; i++) {
        if (visits[i].patient_id == id) {
            printf(" %s | %s | Rx: %s\n", visits[i].date, visits[i].diagnosis, visits[i].prescription);
            found = 1;
        }
    }
    if (!found) printf("[-] No visits found.\n");
}

void showFrequentVisitors() {
    printf("\n--- Frequent Visitors (>3 visits) ---\n");
    int found = 0;
    for (int i = 0; i < p_count; i++) {
        if (patients[i].visit_count > 3) {
            printf(" ID: %-5d | Name: %-15s | Visits: %d\n", 
                   patients[i].id, patients[i].name, patients[i].visit_count);
            found = 1;
        }
    }
    if (!found) printf("[-] No frequent visitors found.\n");
}

void saveDataInteractive() {
    FILE *fp = fopen("patients.txt", "w");
    if (fp) {
        fprintf(fp, "%d\n", p_count);
        for(int i=0; i<p_count; i++) {
            fprintf(fp, "%d %s %d %s %d\n", patients[i].id, patients[i].name, patients[i].age, patients[i].phone, patients[i].visit_count);
        }
        fclose(fp);
    }
    FILE *fv = fopen("visits.txt", "w");
    if (fv) {
        fprintf(fv, "%d\n", v_count);
        for(int i=0; i<v_count; i++) {
            fprintf(fv, "%d %s %s %s\n", visits[i].patient_id, visits[i].date, visits[i].diagnosis, visits[i].prescription);
        }
        fclose(fv);
    }
    printf("[*] Data saved to virtual file system successfully.\n");
}

void loadDataInteractive() {
    FILE *fp = fopen("patients.txt", "r");
    if (fp) {
        if(fscanf(fp, "%d", &p_count) == 1) {
            for(int i=0; i<p_count; i++) {
                fscanf(fp, "%d %s %d %s %d", &patients[i].id, patients[i].name, &patients[i].age, patients[i].phone, &patients[i].visit_count);
            }
        }
        fclose(fp);
    } else {
        printf("[-] No saved patient data found.\n");
    }
    FILE *fv = fopen("visits.txt", "r");
    if (fv) {
        if(fscanf(fv, "%d", &v_count) == 1) {
            for(int i=0; i<v_count; i++) {
                fscanf(fv, "%d %s %s %s", &visits[i].patient_id, visits[i].date, visits[i].diagnosis, visits[i].prescription);
            }
        }
        fclose(fv);
    }
    if (fp || fv) printf("[*] Data loaded successfully.\n");
}

void displayMenu() {
    printf("\n=========================================\n");
    printf(" HOSPITAL PATIENT REGISTRATION & TRACKER\n");
    printf("=========================================\n");
    printf(" 1. Register New Patient\n");
    printf(" 2. Add Visit Record\n");
    printf(" 3. Search Patient by ID\n");
    printf(" 4. Show Visit History\n");
    printf(" 5. Show Frequent Visitors (>3 visits)\n");
    printf(" 6. Save Data to File\n");
    printf(" 7. Load Data from File\n");
    printf(" 0. Exit\n");
    printf("=========================================\n");
    printf("Enter your choice:\n"); 
}

int main() {
    char buffer[256];
    
    // Attempt to load data on startup automatically
    loadDataInteractive();

    while(1) {
        displayMenu();
        get_input(buffer, sizeof(buffer));
        int choice = atoi(buffer);

        switch(choice) {
            case 1: registerPatientInteractive(); break;
            case 2: addVisitInteractive(); break;
            case 3: searchPatientInteractive(); break;
            case 4: showHistoryInteractive(); break;
            case 5: showFrequentVisitors(); break;
            case 6: saveDataInteractive(); break;
            case 7: loadDataInteractive(); break;
            case 0: 
                printf("[*] Exiting System... Goodbye!\n");
                return 0;
            default: printf("[-] Invalid choice.\n");
        }
    }
    return 0;
}