/*  Structures, Arrays, Functions, Pointers, Macros, Stack, Queue, Searching, Linked list */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_RECORDS 50
#define MAX_EXPENSES 100
#define STR 50

typedef struct { char name[STR]; float targetAmount, savedAmount; int deadlineMonths; } Goal;
typedef struct GoalNode { Goal data; struct GoalNode* next; } GoalNode; // Linked List Node
typedef struct { char month[STR]; float income, savings, expenses, budget; } MonthlyRecord;
typedef struct { char category[STR]; char month[STR]; float amount; } Expense;

GoalNode* goalHead = NULL;

int goalCount = 0, recordCount = 0, top = -1, expenseCount = 0, front = 0, rear = -1, queueCount = 0;
MonthlyRecord records[MAX_RECORDS]; // Array
Expense expenseStack[MAX_EXPENSES]; // Stack
Expense pendingQueue[MAX_EXPENSES]; // Queue

/* Utility Functions */
void removeNewline(char *str) { int len = strlen(str); if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0'; }
void getString(char *msg, char *arr, int size) { printf("%s", msg); fgets(arr, size, stdin); removeNewline(arr); }
float getFloat(char *msg) { float v; printf("%s", msg); scanf("%f", &v); getchar(); return v; }
int getInt(char *msg) { int v; printf("%s", msg); scanf("%d", &v); getchar(); return v; }

/* CSV Operations */
void saveToCSV() {
    FILE *fg = fopen("goals.csv", "w");
    if (fg) {
        fprintf(fg, "Name,TargetAmount,SavedAmount,DeadlineMonths\n");
        for (GoalNode* t = goalHead; t; t = t->next)
            fprintf(fg, "%s,%.2f,%.2f,%d\n", t->data.name, t->data.targetAmount, t->data.savedAmount, t->data.deadlineMonths);
        fclose(fg);
    }
    FILE *fr = fopen("records.csv", "w");
    if (fr) {
        fprintf(fr, "Month,Income,Savings,Expenses,Budget\n");
        for (int i = 0; i < recordCount; i++)
            fprintf(fr, "%s,%.2f,%.2f,%.2f,%.2f\n", records[i].month, records[i].income, records[i].savings, records[i].expenses, records[i].budget);
        fclose(fr);
    }
    FILE *fe = fopen("expenses.csv", "w");
    if (fe) {
        fprintf(fe, "Category,Month,Amount\n");
        for (int i = 0; i <= top; i++)
            fprintf(fe, "%s,%s,%.2f\n", expenseStack[i].category, expenseStack[i].month, expenseStack[i].amount);
        fclose(fe);
    }
}

void loadFromCSV() {
    char line[256];
    FILE *fg = fopen("goals.csv", "r");
    if (fg) {
        fgets(line, sizeof(line), fg);
        while (fgets(line, sizeof(line), fg)) {
            GoalNode* n = (GoalNode*)malloc(sizeof(GoalNode));
            sscanf(line, "%[^,],%f,%f,%d", n->data.name, &n->data.targetAmount, &n->data.savedAmount, &n->data.deadlineMonths);
            n->next = NULL;
            if (!goalHead) goalHead = n;
            else { GoalNode* t = goalHead; while(t->next) t = t->next; t->next = n; }
            goalCount++;
        }
        fclose(fg);
    }
    FILE *fr = fopen("records.csv", "r");
    if (fr) {
        fgets(line, sizeof(line), fr);
        for (recordCount = 0; fgets(line, sizeof(line), fr) && recordCount < MAX_RECORDS; recordCount++)
            sscanf(line, "%[^,],%f,%f,%f,%f", records[recordCount].month, &records[recordCount].income, &records[recordCount].savings, &records[recordCount].expenses, &records[recordCount].budget);
        fclose(fr);
    }
    FILE *fe = fopen("expenses.csv", "r");
    if (fe) {
        fgets(line, sizeof(line), fe);
        for (top = -1, expenseCount = 0; fgets(line, sizeof(line), fe) && expenseCount < MAX_EXPENSES; expenseCount++) {
            top++; sscanf(line, "%[^,],%[^,],%f", expenseStack[top].category, expenseStack[top].month, &expenseStack[top].amount);
        }
        fclose(fe);
    }
}

/* Stack Operations */
void pushExpense(Expense e) {
    if (top < MAX_EXPENSES - 1) { expenseStack[++top] = e; expenseCount++; saveToCSV(); }
    else printf("Expense Stack Overflow.\n");
}
void popExpense() {
    if (top == -1) { printf("No Expense To Undo.\n"); return; }
    printf("\nLast Expense Removed: %s | %.2f\n", expenseStack[top].category, expenseStack[top].amount);
    top--; expenseCount--; saveToCSV();
}

/* Queue Operations */
void enqueuePendingExpense() {
    if (queueCount == MAX_EXPENSES) { printf("Pending Queue Full.\n"); return; }
    Expense e; getString("\nEnter Category: ", e.category, STR); getString("Enter Month: ", e.month, STR); e.amount = getFloat("Enter Amount: ");
    if (e.amount < 0) return;
    rear = (rear + 1) % MAX_EXPENSES; pendingQueue[rear] = e; queueCount++;
    printf("Added to Pending Queue.\n");
}
void processPendingExpense() {
    if (queueCount == 0) { printf("No pending expenses.\n"); return; }
    Expense e = pendingQueue[front]; front = (front + 1) % MAX_EXPENSES; queueCount--;
    pushExpense(e); printf("Processed Pending Expense: %s - %.2f\n", e.category, e.amount);
}

/* Linear Search */
GoalNode* findGoal(char *name) { 
    for (GoalNode* t = goalHead; t; t = t->next)
        if (strcmp(t->data.name, name) == 0) return t; // Pointers
    return NULL;
}

/* App Operations */
void addGoal() {
    GoalNode* n = (GoalNode*)malloc(sizeof(GoalNode));
    printf("\n========== ADD GOAL ==========\n");
    getString("Enter Goal Name: ", n->data.name, STR);
    n->data.targetAmount = getFloat("Enter Target Amount: ");
    n->data.savedAmount = getFloat("Enter Saved Amount: ");
    n->data.deadlineMonths = getInt("Enter Deadline (Months): ");
    n->next = NULL;
    if (!goalHead) goalHead = n; else { GoalNode* t = goalHead; while(t->next) t = t->next; t->next = n; }
    goalCount++; saveToCSV(); printf("Goal Added.\n");
}
void searchGoal() {
    char name[STR]; getString("Enter Goal Name: ", name, STR);
    GoalNode* found = findGoal(name);
    if (found) printf("\nFound: %s | Target: %.2f | Saved: %.2f | Months: %d\n", found->data.name, found->data.targetAmount, found->data.savedAmount, found->data.deadlineMonths);
    else printf("Goal Not Found.\n");
}
void updateSavings() {
    char name[STR]; getString("Enter Goal Name To Update: ", name, STR);
    GoalNode* found = findGoal(name);
    if (found) {
        float amount = getFloat("Enter Saving Amount To Add: ");
        if (amount >= 0) { found->data.savedAmount += amount; saveToCSV(); printf("Updated.\n"); }
    } else printf("Goal Not Found.\n");
}
void viewGoals() {
    printf("\n%-20s %-12s %-12s %-10s\n-------------------------------------------------------\n", "Goal", "Target", "Saved", "Months");
    for (GoalNode* t = goalHead; t; t = t->next)
        printf("%-20s %-12.2f %-12.2f %-10d\n", t->data.name, t->data.targetAmount, t->data.savedAmount, t->data.deadlineMonths);
}
void addMonthlyRecord() {
    if (recordCount >= MAX_RECORDS) return;
    getString("\nEnter Month: ", records[recordCount].month, STR);
    records[recordCount].income = getFloat("Enter Income: ");
    records[recordCount].savings = getFloat("Enter Savings: ");
    records[recordCount].expenses = getFloat("Enter Expenses: ");
    records[recordCount].budget = getFloat("Enter Budget Limit: ");
    if (records[recordCount].expenses > records[recordCount].budget) printf("WARNING: Budget Exceeded!\n");
    recordCount++; saveToCSV(); printf("Record Added.\n");
}
void addExpense() {
    Expense e; getString("\nEnter Category: ", e.category, STR); getString("Enter Month: ", e.month, STR); e.amount = getFloat("Enter Amount: ");
    if (e.amount >= 0) { pushExpense(e); printf("Expense Added.\n"); }
}
void viewExpenses() {
    printf("\n%-20s %-15s %-10s\n------------------------------------------------\n", "Category", "Month", "Amount");
    for (int i = 0; i <= top; i++) printf("%-20s %-15s %-10.2f\n", expenseStack[i].category, expenseStack[i].month, expenseStack[i].amount);
}

float getAvg(int type) { // 1=Income, 2=Savings, 3=Expenses
    if (recordCount == 0) return 0;
    float t = 0;
    for (int i = 0; i < recordCount; i++) t += (type==1 ? records[i].income : (type==2 ? records[i].savings : records[i].expenses));
    return t / recordCount;
}

void generateReportAndWarnings() {
    printf("\n============== SAVESPHERE REPORT & WARNINGS ==============\n");
    printf("Goals: %d | Records: %d | Avg Inc: %.2f | Avg Save: %.2f | Avg Exp: %.2f\n", goalCount, recordCount, getAvg(1), getAvg(2), getAvg(3));
    
    for (int i = 0; i < recordCount; i++) {
        if (records[i].expenses > records[i].budget) printf("[!] Budget exceeded in %s\n", records[i].month);
        if (records[i].savings < records[i].income * 0.20) printf("[!] Low savings in %s\n", records[i].month);
    }
    
    for (GoalNode* t = goalHead; t; t = t->next) {
        float rem = t->data.targetAmount - t->data.savedAmount;
        printf("\nGoal: %s | Target: %.2f | Saved: %.2f | Remaining: %.2f", t->data.name, t->data.targetAmount, t->data.savedAmount, rem>0?rem:0);
        if (rem > 0 && getAvg(2) < rem / t->data.deadlineMonths) printf(" -> [WARNING] Behind Schedule!");
    }
    printf("\n==========================================================\n");
}

int main() {
    loadFromCSV();
    int choice;
    char menu[] = "\n--- SAVESPHERE ---\n1.Add Goal 2.View Goals 3.Update Savings 4.Add Record\n5.Add Expense 6.View Expenses 7.Undo Expense\n8.Queue Pending Exp 9.Process Pending Exp\n10.Search Goal 11.Report & Warnings 12.Exit\nChoice: ";
    
    while (1) {
        choice = getInt(menu);
        if (choice == 1) addGoal(); else if (choice == 2) viewGoals();
        else if (choice == 3) updateSavings(); else if (choice == 4) addMonthlyRecord();
        else if (choice == 5) addExpense(); else if (choice == 6) viewExpenses();
        else if (choice == 7) popExpense(); else if (choice == 8) enqueuePendingExpense();
        else if (choice == 9) processPendingExpense(); else if (choice == 10) searchGoal();
        else if (choice == 11) generateReportAndWarnings();
        else if (choice == 12) {
            printf("\nSaving to GitHub...\n"); system("git add *.csv"); system("git commit -m \"Auto-update\""); system("git push");
            return 0;
        } else printf("Invalid.\n");
    }
}
