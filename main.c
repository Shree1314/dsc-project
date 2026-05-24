/*  Structures, Arrays, Functions, Pointers, Macros, Stack, Queue, Searching, Linked list
    Used to store related financial data together
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*  Macros  */
#define MAX_RECORDS 50
#define MAX_EXPENSES 100
#define STR 50

/*  Structures  */
typedef struct {
    char name[STR];
    float targetAmount;
    float savedAmount;
    int deadlineMonths;
} Goal;

/*  Linked List Node Structure (Pointers) */
typedef struct GoalNode {
    Goal data;
    struct GoalNode* next;
} GoalNode;

typedef struct {
    char month[STR];
    float income;
    float savings;
    float expenses;
    float budget;
} MonthlyRecord;

typedef struct {
    char category[STR];
    char month[STR];
    float amount;
} Expense;

/*  Arrays & Data Structures  */

// Linked List Head
GoalNode* goalHead = NULL;
int goalCount = 0;

// Array
MonthlyRecord records[MAX_RECORDS];
int recordCount = 0;

// Stack (for undoing expenses)
Expense expenseStack[MAX_EXPENSES];
int top = -1;
int expenseCount = 0;

// Queue (for pending expenses)
Expense pendingQueue[MAX_EXPENSES];
int front = 0;
int rear = -1;
int queueCount = 0;

/* --- Functions & Pointers --- */

void removeNewline(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

void getString(char message[], char *arr, int size) {
    printf("%s", message);
    fgets(arr, size, stdin);
    removeNewline(arr);
}

float getFloat(char message[]) {
    float value;
    printf("%s", message);
    scanf("%f", &value);
    getchar();
    return value;
}

int getInt(char message[]) {
    int value;
    printf("%s", message);
    scanf("%d", &value);
    getchar();
    return value;
}

/* --- CSV FILE OPERATIONS --- */

void saveToCSV() {
    // Save Goals (Linked List Iteration)
    FILE *fg = fopen("goals.csv", "w");
    if (fg != NULL) {
        fprintf(fg, "Name,TargetAmount,SavedAmount,DeadlineMonths\n");
        GoalNode* temp = goalHead;
        while (temp != NULL) {
            fprintf(fg, "%s,%.2f,%.2f,%d\n", temp->data.name, temp->data.targetAmount, temp->data.savedAmount, temp->data.deadlineMonths);
            temp = temp->next;
        }
        fclose(fg);
    }

    // Save Records (Array Iteration)
    FILE *fr = fopen("records.csv", "w");
    if (fr != NULL) {
        fprintf(fr, "Month,Income,Savings,Expenses,Budget\n");
        for (int i = 0; i < recordCount; i++) {
            fprintf(fr, "%s,%.2f,%.2f,%.2f,%.2f\n", records[i].month, records[i].income, records[i].savings, records[i].expenses, records[i].budget);
        }
        fclose(fr);
    }

    // Save Expenses (Stack Iteration)
    FILE *fe = fopen("expenses.csv", "w");
    if (fe != NULL) {
        fprintf(fe, "Category,Month,Amount\n");
        for (int i = 0; i <= top; i++) {
            fprintf(fe, "%s,%s,%.2f\n", expenseStack[i].category, expenseStack[i].month, expenseStack[i].amount);
        }
        fclose(fe);
    }
}

void loadFromCSV() {
    char line[256];

    // Load Goals (Insert into Linked List)
    FILE *fg = fopen("goals.csv", "r");
    if (fg != NULL) {
        fgets(line, sizeof(line), fg); // skip header
        while (fgets(line, sizeof(line), fg)) {
            Goal g;
            sscanf(line, "%[^,],%f,%f,%d", g.name, &g.targetAmount, &g.savedAmount, &g.deadlineMonths);
            
            GoalNode* newNode = (GoalNode*)malloc(sizeof(GoalNode));
            newNode->data = g;
            newNode->next = NULL;

            if (goalHead == NULL) {
                goalHead = newNode;
            } else {
                GoalNode* temp = goalHead;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = newNode;
            }
            goalCount++;
        }
        fclose(fg);
    }

    // Load Records (Array)
    FILE *fr = fopen("records.csv", "r");
    if (fr != NULL) {
        fgets(line, sizeof(line), fr); // skip header
        recordCount = 0;
        while (fgets(line, sizeof(line), fr) && recordCount < MAX_RECORDS) {
            sscanf(line, "%[^,],%f,%f,%f,%f", records[recordCount].month, &records[recordCount].income, &records[recordCount].savings, &records[recordCount].expenses, &records[recordCount].budget);
            recordCount++;
        }
        fclose(fr);
    }

    // Load Expenses (Stack)
    FILE *fe = fopen("expenses.csv", "r");
    if (fe != NULL) {
        fgets(line, sizeof(line), fe); // skip header
        top = -1;
        expenseCount = 0;
        while (fgets(line, sizeof(line), fe) && expenseCount < MAX_EXPENSES) {
            top++;
            sscanf(line, "%[^,],%[^,],%f", expenseStack[top].category, expenseStack[top].month, &expenseStack[top].amount);
            expenseCount++;
        }
        fclose(fe);
    }
}

/* --- STACK OPERATIONS --- */

void pushExpense(Expense e) {
    if (top == MAX_EXPENSES - 1) {
        printf("Expense Stack Overflow.\n");
        return;
    }
    top++;
    expenseStack[top] = e;
    expenseCount++;
}

void popExpense() {
    if (top == -1) {
        printf("No Expense To Undo.\n");
        return;
    }
    printf("\nLast Expense Removed Successfully.\n");
    printf("Removed Expense: %s | %.2f\n", expenseStack[top].category, expenseStack[top].amount);
    top--;
    expenseCount--;
    saveToCSV(); // Save state after undo
}

/* --- QUEUE OPERATIONS --- */

void enqueuePendingExpense() {
    if (queueCount == MAX_EXPENSES) {
        printf("Pending Queue is Full.\n");
        return;
    }
    Expense e;
    printf("\n======= ADD PENDING EXPENSE (QUEUE) =======\n");
    getString("Enter Expense Category: ", e.category, STR);
    getString("Enter Month: ", e.month, STR);
    e.amount = getFloat("Enter Expense Amount: ");

    if (e.amount < 0) {
        printf("Invalid Amount.\n");
        return;
    }

    rear = (rear + 1) % MAX_EXPENSES;
    pendingQueue[rear] = e;
    queueCount++;
    printf("Added to Pending Queue.\n");
}

void processPendingExpense() {
    if (queueCount == 0) {
        printf("No pending expenses to process.\n");
        return;
    }
    
    Expense e = pendingQueue[front];
    front = (front + 1) % MAX_EXPENSES;
    queueCount--;

    pushExpense(e); // Move to real expenses stack
    saveToCSV();
    
    printf("Processed Pending Expense: %s - %.2f\n", e.category, e.amount);
}

/* --- LINKED LIST OPERATIONS --- */

void addGoal() {
    Goal g;
    printf("\n========== ADD GOAL (LINKED LIST) ==========\n");
    getString("Enter Goal Name: ", g.name, STR);
    g.targetAmount = getFloat("Enter Target Amount: ");
    g.savedAmount = getFloat("Enter Saved Amount: ");
    g.deadlineMonths = getInt("Enter Deadline (Months): ");

    if (g.targetAmount <= 0 || g.savedAmount < 0 || g.deadlineMonths <= 0) {
        printf("Invalid Goal Data.\n");
        return;
    }

    // Allocate new node (Pointers)
    GoalNode* newNode = (GoalNode*)malloc(sizeof(GoalNode));
    newNode->data = g;
    newNode->next = NULL;

    if (goalHead == NULL) {
        goalHead = newNode;
    } else {
        GoalNode* temp = goalHead;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    
    goalCount++;
    saveToCSV(); // Save state
    printf("Goal Added Successfully.\n");
}

void viewGoals() {
    if (goalHead == NULL) {
        printf("No Goals Available.\n");
        return;
    }
    printf("\n================ GOALS ================\n");
    printf("%-20s %-12s %-12s %-12s %-10s\n", "Goal", "Target", "Saved", "Remaining", "Months");
    printf("-------------------------------------------------------------\n");
    
    GoalNode* temp = goalHead;
    while (temp != NULL) {
        float remaining = temp->data.targetAmount - temp->data.savedAmount;
        if (remaining < 0) remaining = 0;
        printf("%-20s %-12.2f %-12.2f %-12.2f %-10d\n", temp->data.name, temp->data.targetAmount, temp->data.savedAmount, remaining, temp->data.deadlineMonths);
        temp = temp->next;
    }
}

void updateSavings() {
    char name[STR];
    int found = 0;
    getString("Enter Goal Name To Update: ", name, STR);
    
    // Linear Search on Linked List
    GoalNode* temp = goalHead;
    while (temp != NULL) {
        if (strcmp(temp->data.name, name) == 0) {
            float amount = getFloat("Enter Saving Amount To Add: ");
            if (amount < 0) {
                printf("Invalid Amount.\n");
                return;
            }
            temp->data.savedAmount += amount;
            printf("Savings Updated Successfully.\n");
            saveToCSV(); // Save state
            found = 1;
            break;
        }
        temp = temp->next;
    }
    if (!found) {
        printf("Goal Not Found.\n");
    }
}

/* --- ARRAY OPERATIONS --- */

void addMonthlyRecord() {
    if (recordCount >= MAX_RECORDS) {
        printf("Monthly Record Storage Full.\n");
        return;
    }
    printf("\n======= ADD MONTHLY RECORD =======\n");
    getString("Enter Month: ", records[recordCount].month, STR);
    records[recordCount].income = getFloat("Enter Monthly Income: ");
    records[recordCount].savings = getFloat("Enter Monthly Savings: ");
    records[recordCount].expenses = getFloat("Enter Monthly Expenses: ");
    records[recordCount].budget = getFloat("Enter Budget Limit: ");

    if (records[recordCount].income < 0 || records[recordCount].savings < 0 || records[recordCount].expenses < 0 || records[recordCount].budget < 0) {
        printf("Invalid Record Data.\n");
        return;
    }
    if (records[recordCount].expenses > records[recordCount].budget) {
        printf("\nWARNING: Budget Exceeded By %.2f\n", records[recordCount].expenses - records[recordCount].budget);
    }

    recordCount++;
    saveToCSV(); // Save state
    printf("Monthly Record Added Successfully.\n");
}

void addExpense() {
    Expense e;
    printf("\n========== ADD EXPENSE ==========\n");
    getString("Enter Expense Category: ", e.category, STR);
    getString("Enter Month: ", e.month, STR);
    e.amount = getFloat("Enter Expense Amount: ");

    if (e.amount < 0) {
        printf("Invalid Amount.\n");
        return;
    }

    pushExpense(e);
    saveToCSV(); // Save state
    printf("Expense Added Successfully.\n");
}

void viewExpenses() {
    if (top == -1) {
        printf("No Expenses Available.\n");
        return;
    }
    printf("\n=============== EXPENSES ===============\n");
    printf("%-20s %-15s %-10s\n", "Category", "Month", "Amount");
    printf("------------------------------------------------\n");
    for (int i = 0; i <= top; i++) {
        printf("%-20s %-15s %-10.2f\n", expenseStack[i].category, expenseStack[i].month, expenseStack[i].amount);
    }
}

/* --- STATISTICS & REPORTS --- */

float averageSavings() {
    if (recordCount == 0) return 0;
    float total = 0;
    for (int i = 0; i < recordCount; i++) total += records[i].savings;
    return total / recordCount;
}

float averageIncome() {
    if (recordCount == 0) return 0;
    float total = 0;
    for (int i = 0; i < recordCount; i++) total += records[i].income;
    return total / recordCount;
}

float averageExpenses() {
    if (recordCount == 0) return 0;
    float total = 0;
    for (int i = 0; i < recordCount; i++) total += records[i].expenses;
    return total / recordCount;
}

void checkWarnings() {
    int warning = 0;
    printf("\n=========== WARNING SYSTEM ===========\n");
    for (int i = 0; i < recordCount; i++) {
        if (records[i].expenses > records[i].budget) {
            printf("Budget Exceeded In %s\n", records[i].month);
            warning = 1;
        }
        if (records[i].savings < (records[i].income * 0.20)) {
            printf("Low Savings In %s\n", records[i].month);
            warning = 1;
        }
    }
    float avgSave = averageSavings();
    
    GoalNode* temp = goalHead;
    while (temp != NULL) {
        float remaining = temp->data.targetAmount - temp->data.savedAmount;
        if (remaining > 0) {
            float required = remaining / temp->data.deadlineMonths;
            if (avgSave < required) {
                printf("Goal '%s' May Miss Deadline\n", temp->data.name);
                warning = 1;
            }
        }
        temp = temp->next;
    }
    
    if (!warning) printf("No Warnings.\n");
}

void generateReport() {
    printf("\n=========================================\n");
    printf("           GOALFUND REPORT\n");
    printf("=========================================\n");
    printf("Total Goals       : %d\n", goalCount);
    printf("Total Expenses    : %d\n", expenseCount);
    printf("Monthly Records   : %d\n", recordCount);
    printf("\nAverage Income    : %.2f\n", averageIncome());
    printf("Average Savings   : %.2f\n", averageSavings());
    printf("Average Expenses  : %.2f\n", averageExpenses());
    printf("\n============= GOAL STATUS =============\n");
    
    GoalNode* temp = goalHead;
    while (temp != NULL) {
        float remaining = temp->data.targetAmount - temp->data.savedAmount;
        if (remaining < 0) remaining = 0;
        printf("\nGoal Name : %s\n", temp->data.name);
        printf("Target    : %.2f\n", temp->data.targetAmount);
        printf("Saved     : %.2f\n", temp->data.savedAmount);
        printf("Remaining : %.2f\n", remaining);
        printf("Deadline  : %d Months\n", temp->data.deadlineMonths);
        
        if (remaining == 0) {
            printf("Status    : COMPLETED\n");
        } else {
            float required = remaining / temp->data.deadlineMonths;
            printf("Need/Month: %.2f\n", required);
            if (averageSavings() >= required) {
                printf("Status    : ON TRACK\n");
            } else {
                printf("Status    : BEHIND SCHEDULE\n");
            }
        }
        temp = temp->next;
    }
    printf("\n=========================================\n");
}

/* --- SEARCHING --- */

void searchGoal() {
    char name[STR];
    int found = 0;
    getString("Enter Goal Name To Search: ", name, STR);
    
    // Linear Search on Linked List
    GoalNode* temp = goalHead;
    while (temp != NULL) {
        if (strcmp(temp->data.name, name) == 0) {
            printf("\nGoal Found.\n");
            printf("Goal Name : %s\n", temp->data.name);
            printf("Target    : %.2f\n", temp->data.targetAmount);
            printf("Saved     : %.2f\n", temp->data.savedAmount);
            printf("Deadline  : %d Months\n", temp->data.deadlineMonths);
            found = 1;
            break;
        }
        temp = temp->next;
    }
    if (!found) printf("Goal Not Found.\n");
}

/* --- MAIN MENU --- */

void showMenu() {
    printf("\n=================================\n");
    printf("            GOALFUND\n");
    printf("=================================\n");
    printf("1. Add Savings Goal (Linked List)\n");
    printf("2. View Goals\n");
    printf("3. Update Savings\n");
    printf("4. Add Monthly Record (Array)\n");
    printf("5. Add Direct Expense (Stack)\n");
    printf("6. View Expenses\n");
    printf("7. Undo Last Expense (Stack Pop)\n");
    printf("8. Add Pending Expense (Queue)\n");
    printf("9. Process Pending Expense (Queue)\n");
    printf("10. Search Goal (Linear Search)\n");
    printf("11. Check Warnings\n");
    printf("12. Generate Report\n");
    printf("13. Exit\n");
    printf("=================================\n");
}

int main() {
    // Load existing data from CSV on startup
    loadFromCSV();

    int choice;
    printf("\nWELCOME TO GOALFUND SYSTEM\n");

    while (1) {
        showMenu();
        choice = getInt("Enter Your Choice: ");
        switch (choice) {
            case 1: addGoal(); break;
            case 2: viewGoals(); break;
            case 3: updateSavings(); break;
            case 4: addMonthlyRecord(); break;
            case 5: addExpense(); break;
            case 6: viewExpenses(); break;
            case 7: popExpense(); break;
            case 8: enqueuePendingExpense(); break;
            case 9: processPendingExpense(); break;
            case 10: searchGoal(); break;
            case 11: checkWarnings(); break;
            case 12: generateReport(); break;
            case 13: 
                printf("\nThank You For Using GoalFund.\n");
                return 0;
            default:
                printf("Invalid Choice.\n");
        }
    }
    return 0;
}
