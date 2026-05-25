#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX_RECORDS 50
#define MAX_EXPENSES 100
#define STR 50

typedef struct{
    char name[STR];
    float targetAmount,savedAmount;
    int deadlineMonths;
}Goal;

typedef struct GoalNode{
    Goal data;
    struct GoalNode*next;
}GoalNode;

typedef struct{
    char month[STR];
    float income,savings,expenses,budget;
}MonthlyRecord;

typedef struct{
    char category[STR],month[STR];
    float amount;
}Expense;

GoalNode*goalHead=NULL;
MonthlyRecord records[MAX_RECORDS];
Expense expenseStack[MAX_EXPENSES];

int goalCount=0,recordCount=0,top=-1,expenseCount=0;

void removeNewline(char*s){
    int l=strlen(s);
    if(l&&s[l-1]=='\n') s[l-1]='\0';
}

void getString(char msg[],char*arr,int size){
    printf("%s",msg);
    fgets(arr,size,stdin);
    removeNewline(arr);
}

float getFloat(char msg[]){
    float v;
    printf("%s",msg);
    scanf("%f",&v);
    getchar();
    return v;
}

int getInt(char msg[]){
    int v;
    printf("%s",msg);
    scanf("%d",&v);
    getchar();
    return v;
}

/* CSV FILES */

void saveToCSV(){

    FILE *fg=fopen("goals.csv","w");
    if(fg){
        fprintf(fg,"Name,TargetAmount,SavedAmount,DeadlineMonths\n");

        GoalNode*t=goalHead;

        while(t){
            fprintf(fg,"%s,%.2f,%.2f,%d\n",
            t->data.name,
            t->data.targetAmount,
            t->data.savedAmount,
            t->data.deadlineMonths);

            t=t->next;
        }
        fclose(fg);
    }

    FILE *fr=fopen("records.csv","w");

    if(fr){
        fprintf(fr,"Month,Income,Savings,Expenses,Budget\n");

        for(int i=0;i<recordCount;i++){
            fprintf(fr,"%s,%.2f,%.2f,%.2f,%.2f\n",
            records[i].month,
            records[i].income,
            records[i].savings,
            records[i].expenses,
            records[i].budget);
        }

        fclose(fr);
    }

    FILE *fe=fopen("expenses.csv","w");

    if(fe){
        fprintf(fe,"Category,Month,Amount\n");

        for(int i=0;i<=top;i++){
            fprintf(fe,"%s,%s,%.2f\n",
            expenseStack[i].category,
            expenseStack[i].month,
            expenseStack[i].amount);
        }

        fclose(fe);
    }
}

void loadFromCSV(){

    char line[256];

    FILE *fg=fopen("goals.csv","r");

    if(fg){

        fgets(line,sizeof(line),fg);

        while(fgets(line,sizeof(line),fg)){

            Goal g;

            sscanf(line,"%[^,],%f,%f,%d",
            g.name,
            &g.targetAmount,
            &g.savedAmount,
            &g.deadlineMonths);

            GoalNode*newNode=(GoalNode*)malloc(sizeof(GoalNode));

            newNode->data=g;
            newNode->next=NULL;

            if(!goalHead) goalHead=newNode;

            else{
                GoalNode*t=goalHead;

                while(t->next) t=t->next;

                t->next=newNode;
            }

            goalCount++;
        }

        fclose(fg);
    }

    FILE *fr=fopen("records.csv","r");

    if(fr){

        fgets(line,sizeof(line),fr);

        while(fgets(line,sizeof(line),fr)&&recordCount<MAX_RECORDS){

            sscanf(line,"%[^,],%f,%f,%f,%f",
            records[recordCount].month,
            &records[recordCount].income,
            &records[recordCount].savings,
            &records[recordCount].expenses,
            &records[recordCount].budget);

            recordCount++;
        }

        fclose(fr);
    }

    FILE *fe=fopen("expenses.csv","r");

    if(fe){

        fgets(line,sizeof(line),fe);

        while(fgets(line,sizeof(line),fe)&&expenseCount<MAX_EXPENSES){

            top++;

            sscanf(line,"%[^,],%[^,],%f",
            expenseStack[top].category,
            expenseStack[top].month,
            &expenseStack[top].amount);

            expenseCount++;
        }

        fclose(fe);
    }
}

/* STACK */

void pushExpense(Expense e){

    if(top==MAX_EXPENSES-1){
        printf("Expense Stack Full.\n");
        return;
    }

    expenseStack[++top]=e;
    expenseCount++;
}

/* GOALS */

void addGoal(){

    Goal g;

    printf("\n===== ADD GOAL =====\n");

    getString("Enter Goal Name: ",g.name,STR);

    g.targetAmount=getFloat("Enter Target Amount: ");
    g.savedAmount=getFloat("Enter Saved Amount: ");
    g.deadlineMonths=getInt("Enter Deadline(Months): ");

    if(g.targetAmount<=0||g.savedAmount<0||g.deadlineMonths<=0){
        printf("Invalid Goal Data.\n");
        return;
    }

    GoalNode*newNode=(GoalNode*)malloc(sizeof(GoalNode));

    newNode->data=g;
    newNode->next=NULL;

    if(!goalHead) goalHead=newNode;

    else{

        GoalNode*t=goalHead;

        while(t->next) t=t->next;

        t->next=newNode;
    }

    goalCount++;

    saveToCSV();

    printf("Goal Added Successfully.\n");
}

void viewGoals(){

    if(!goalHead){
        printf("No Goals Available.\n");
        return;
    }

    GoalNode*t=goalHead;

    printf("\n=========== GOALS ===========\n");

    while(t){

        float remain=t->data.targetAmount-t->data.savedAmount;

        if(remain<0) remain=0;

        printf("\nGoal      : %s",t->data.name);
        printf("\nTarget    : %.2f",t->data.targetAmount);
        printf("\nSaved     : %.2f",t->data.savedAmount);
        printf("\nRemaining : %.2f",remain);
        printf("\nDeadline  : %d Months\n",t->data.deadlineMonths);

        t=t->next;
    }
}

void updateSavings(){

    char name[STR];

    getString("Enter Goal Name: ",name,STR);

    GoalNode*t=goalHead;

    while(t){

        if(strcmp(t->data.name,name)==0){

            float amt=getFloat("Enter Amount To Add: ");

            if(amt<0){
                printf("Invalid Amount.\n");
                return;
            }

            t->data.savedAmount+=amt;

            saveToCSV();

            printf("Savings Updated.\n");

            return;
        }

        t=t->next;
    }

    printf("Goal Not Found.\n");
}

/* MONTHLY RECORD */

void addMonthlyRecord(){

    if(recordCount>=MAX_RECORDS){
        printf("Storage Full.\n");
        return;
    }

    printf("\n===== ADD MONTHLY RECORD =====\n");

    getString("Enter Month: ",records[recordCount].month,STR);

    records[recordCount].income=getFloat("Enter Income: ");
    records[recordCount].savings=getFloat("Enter Savings: ");
    records[recordCount].expenses=getFloat("Enter Expenses: ");
    records[recordCount].budget=getFloat("Enter Budget: ");

    if(records[recordCount].expenses>
       records[recordCount].budget){

        printf("WARNING: Budget Exceeded.\n");
    }

    recordCount++;

    saveToCSV();

    printf("Record Added Successfully.\n");
}

/* EXPENSE */

void addExpense(){

    Expense e;

    printf("\n===== ADD EXPENSE =====\n");

    getString("Enter Category: ",e.category,STR);
    getString("Enter Month: ",e.month,STR);

    e.amount=getFloat("Enter Amount: ");

    if(e.amount<0){
        printf("Invalid Amount.\n");
        return;
    }

    pushExpense(e);

    saveToCSV();

    printf("Expense Added.\n");
}

void viewExpenses(){

    if(top==-1){
        printf("No Expenses Available.\n");
        return;
    }

    printf("\n=========== EXPENSES ===========\n");

    for(int i=0;i<=top;i++){

        printf("\nCategory : %s",expenseStack[i].category);
        printf("\nMonth    : %s",expenseStack[i].month);
        printf("\nAmount   : %.2f\n",expenseStack[i].amount);
    }
}

/* AVERAGES */

float avg(int type){

    if(!recordCount) return 0;

    float t=0;

    for(int i=0;i<recordCount;i++){

        if(type==0) t+=records[i].income;
        else if(type==1) t+=records[i].savings;
        else t+=records[i].expenses;
    }

    return t/recordCount;
}

/* WARNING */

void checkWarnings(){

    int warning=0;

    printf("\n======= WARNINGS =======\n");

    for(int i=0;i<recordCount;i++){

        if(records[i].expenses>records[i].budget){

            printf("Budget Exceeded In %s\n",
            records[i].month);

            warning=1;
        }

        if(records[i].savings<
           records[i].income*0.20){

            printf("Low Savings In %s\n",
            records[i].month);

            warning=1;
        }
    }

    if(!warning) printf("No Warnings.\n");
}

/* REPORT */

void generateReport(){

    printf("\n========= REPORT =========\n");

    printf("Goals          : %d\n",goalCount);
    printf("Expenses       : %d\n",expenseCount);
    printf("MonthlyRecords : %d\n",recordCount);

    printf("\nAverage Income   : %.2f",avg(0));
    printf("\nAverage Savings  : %.2f",avg(1));
    printf("\nAverage Expenses : %.2f\n",avg(2));
}

/* SEARCH */

void searchGoal(){

    char name[STR];

    getString("Enter Goal Name: ",name,STR);

    GoalNode*t=goalHead;

    while(t){

        if(strcmp(t->data.name,name)==0){

            printf("\nGoal Found\n");

            printf("Goal Name : %s\n",t->data.name);
            printf("Target    : %.2f\n",t->data.targetAmount);
            printf("Saved     : %.2f\n",t->data.savedAmount);
            printf("Deadline  : %d Months\n",
            t->data.deadlineMonths);

            return;
        }

        t=t->next;
    }

    printf("Goal Not Found.\n");
}

/* MENU */

void showMenu(){

    printf("\n=============================\n");
    printf("        SAVESPHERE\n");
    printf("=============================\n");

    printf("1. Add Savings Goal\n");
    printf("2. View Goals\n");
    printf("3. Update Savings\n");
    printf("4. Add Monthly Record\n");
    printf("5. Add Direct Expense\n");
    printf("6. View Expenses\n");
    printf("7. Search Goal\n");
    printf("8. Check Warnings\n");
    printf("9. Generate Report\n");
    printf("10. Exit\n");

    printf("=============================\n");
}

int main(){

    loadFromCSV();

    int choice;

    printf("\nWELCOME TO SAVESPHERE\n");

    while(1){

        showMenu();

        choice=getInt("Enter Choice: ");

        switch(choice){

            case 1:addGoal();break;
            case 2:viewGoals();break;
            case 3:updateSavings();break;
            case 4:addMonthlyRecord();break;
            case 5:addExpense();break;
            case 6:viewExpenses();break;
            case 7:searchGoal();break;
            case 8:checkWarnings();break;
            case 9:generateReport();break;

            case 10:

                printf("\nSaving database...\n");

                system("git add *.csv");
                system("git commit -m \"Auto-update\"");
                system("git push");

                printf("\nThank You For Using SaveSphere.\n");

                return 0;

            default:
                printf("Invalid Choice.\n");
        }
    }

    return 0;
}
