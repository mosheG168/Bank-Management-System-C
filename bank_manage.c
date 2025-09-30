#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_NAME 100
#define MAX_ACCOUNTS 1000
#define FILENAME "accounts.dat"
#define TRANSACTION_FILE "transactions.dat"
#define MIN_BALANCE 100.0

typedef struct {
    int accountNumber;
    char name[MAX_NAME];
    char accountType[20];
    double balance;
    char dateCreated[20];
} Account;

typedef struct {
    int accountNumber;
    char type[20];          
    double amount;
    double balanceAfter;
    char date[20];
    char time[20];
} Transaction;

void mainMenu();
void createAccount();
void deposit();
void withdraw();
void checkBalance();
void viewAllAccounts();
void deleteAccount();
void modifyAccount();
void searchByName();
void viewTransactionHistory();
int findAccount(int accNum, Account *acc);
void getCurrentDate(char *date);
void getCurrentTime(char *timeStr);
void clearInputBuffer();
void displayHeader();
void addTransaction(int accNum, char *type, double amount, double balanceAfter);

int main() {
    int choice;
    
    while(1) {
        mainMenu();
        printf("\nEnter your choice: ");
        
        if(scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("\nInvalid input! Please enter a number.\n");
            printf("Press Enter to continue...");
            getchar();
            continue;
        }
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                createAccount();
                break;
            case 2:
                deposit();
                break;
            case 3:
                withdraw();
                break;
            case 4:
                checkBalance();
                break;
            case 5:
                viewAllAccounts();
                break;
            case 6:
                searchByName();
                break;
            case 7:
                viewTransactionHistory();
                break;
            case 8:
                modifyAccount();
                break;
            case 9:
                deleteAccount();
                break;
            case 10:
                printf("\n\n");
                displayHeader();
                printf("Thank you for using Bank Management System!\n");
                printf("Goodbye!\n");
                printf("=========================================\n\n");
                exit(0);
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
        
        printf("\nPress Enter to continue...");
        getchar();
    }
    
    return 0;
}

void displayHeader() {
    printf("\n=========================================\n");
    printf("     BANK MANAGEMENT SYSTEM\n");
    printf("=========================================\n");
}

void mainMenu() {
    system("clear || cls");
    displayHeader();
    printf("1.  Create New Account\n");
    printf("2.  Deposit Money\n");
    printf("3.  Withdraw Money\n");
    printf("4.  Check Balance\n");
    printf("5.  View All Accounts\n");
    printf("6.  Search Account by Name\n");
    printf("7.  View Transaction History\n");
    printf("8.  Modify Account\n");
    printf("9.  Delete Account\n");
    printf("10. Exit\n");
    printf("=========================================\n");
}

void getCurrentDate(char *date) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

void getCurrentTime(char *timeStr) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(timeStr, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void addTransaction(int accNum, char *type, double amount, double balanceAfter) {
    Transaction trans;
    FILE *fp;
    
    trans.accountNumber = accNum;
    strcpy(trans.type, type);
    trans.amount = amount;
    trans.balanceAfter = balanceAfter;
    getCurrentDate(trans.date);
    getCurrentTime(trans.time);
    
    fp = fopen(TRANSACTION_FILE, "ab");
    if(fp != NULL) {
        fwrite(&trans, sizeof(Transaction), 1, fp);
        fclose(fp);
    }
}

void createAccount() {
    Account acc;
    FILE *fp;
    int lastAccNum = 1000;
    
    system("clear || cls");
    displayHeader();
    printf("CREATE NEW ACCOUNT\n");
    printf("-----------------------------------------\n");
    
    // Read last account number
    fp = fopen(FILENAME, "rb");
    if(fp != NULL) {
        Account temp;
        while(fread(&temp, sizeof(Account), 1, fp)) {
            if(temp.accountNumber >= lastAccNum) {
                lastAccNum = temp.accountNumber;
            }
        }
        fclose(fp);
    }
    
    acc.accountNumber = lastAccNum + 1;
    
    printf("Account Number: %d\n", acc.accountNumber);
    
    printf("Enter Account Holder Name: ");
    fgets(acc.name, MAX_NAME, stdin);
    acc.name[strcspn(acc.name, "\n")] = 0;
    
    printf("Enter Account Type (Savings/Current): ");
    fgets(acc.accountType, 20, stdin);
    acc.accountType[strcspn(acc.accountType, "\n")] = 0;
    
    printf("Enter Initial Deposit Amount: $");
    if(scanf("%lf", &acc.balance) != 1 || acc.balance < 0) {
        printf("\nInvalid amount! Account creation failed.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    if(acc.balance < MIN_BALANCE) {
        printf("\nMinimum initial deposit is $%.2f!\n", MIN_BALANCE);
        return;
    }
    
    getCurrentDate(acc.dateCreated);
    
    // Save to file
    fp = fopen(FILENAME, "ab");
    if(fp == NULL) {
        printf("\nError opening file!\n");
        return;
    }
    
    fwrite(&acc, sizeof(Account), 1, fp);
    fclose(fp);
    
    // Add transaction record
    addTransaction(acc.accountNumber, "ACCOUNT_CREATED", acc.balance, acc.balance);
    
    printf("\n-----------------------------------------\n");
    printf("Account created successfully!\n");
    printf("Your Account Number is: %d\n", acc.accountNumber);
    printf("Please remember this number for future transactions.\n");
}

int findAccount(int accNum, Account *acc) {
    FILE *fp = fopen(FILENAME, "rb");
    if(fp == NULL) {
        return 0;
    }
    
    while(fread(acc, sizeof(Account), 1, fp)) {
        if(acc->accountNumber == accNum) {
            fclose(fp);
            return 1;
        }
    }
    
    fclose(fp);
    return 0;
}

void deposit() {
    int accNum;
    double amount;
    Account acc;
    FILE *fp, *temp;
    
    system("clear || cls");
    displayHeader();
    printf("DEPOSIT MONEY\n");
    printf("-----------------------------------------\n");
    
    printf("Enter Account Number: ");
    if(scanf("%d", &accNum) != 1) {
        printf("\nInvalid account number!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    if(!findAccount(accNum, &acc)) {
        printf("\nAccount not found!\n");
        return;
    }
    
    printf("\nAccount Holder: %s\n", acc.name);
    printf("Current Balance: $%.2f\n", acc.balance);
    
    printf("\nEnter amount to deposit: $");
    if(scanf("%lf", &amount) != 1 || amount <= 0) {
        printf("\nInvalid amount!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    // Update account
    fp = fopen(FILENAME, "rb");
    temp = fopen("temp.dat", "wb");
    
    if(fp == NULL || temp == NULL) {
        printf("\nError opening file!\n");
        if(fp) fclose(fp);
        if(temp) fclose(temp);
        return;
    }
    
    while(fread(&acc, sizeof(Account), 1, fp)) {
        if(acc.accountNumber == accNum) {
            acc.balance += amount;
            addTransaction(accNum, "DEPOSIT", amount, acc.balance);
        }
        fwrite(&acc, sizeof(Account), 1, temp);
    }
    
    fclose(fp);
    fclose(temp);
    
    remove(FILENAME);
    rename("temp.dat", FILENAME);
    
    printf("\n-----------------------------------------\n");
    printf("Deposit successful!\n");
    printf("New Balance: $%.2f\n", acc.balance);
}

void withdraw() {
    int accNum;
    double amount;
    Account acc;
    FILE *fp, *temp;
    
    system("clear || cls");
    displayHeader();
    printf("WITHDRAW MONEY\n");
    printf("-----------------------------------------\n");
    
    printf("Enter Account Number: ");
    if(scanf("%d", &accNum) != 1) {
        printf("\nInvalid account number!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    if(!findAccount(accNum, &acc)) {
        printf("\nAccount not found!\n");
        return;
    }
    
    printf("\nAccount Holder: %s\n", acc.name);
    printf("Current Balance: $%.2f\n", acc.balance);
    
    printf("\nEnter amount to withdraw: $");
    if(scanf("%lf", &amount) != 1 || amount <= 0) {
        printf("\nInvalid amount!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    if(amount > acc.balance) {
        printf("\nInsufficient balance!\n");
        return;
    }
    
    if(acc.balance - amount < MIN_BALANCE) {
        printf("\nMinimum balance of $%.2f must be maintained!\n", MIN_BALANCE);
        return;
    }
    
    // Update account
    fp = fopen(FILENAME, "rb");
    temp = fopen("temp.dat", "wb");
    
    if(fp == NULL || temp == NULL) {
        printf("\nError opening file!\n");
        if(fp) fclose(fp);
        if(temp) fclose(temp);
        return;
    }
    
    while(fread(&acc, sizeof(Account), 1, fp)) {
        if(acc.accountNumber == accNum) {
            acc.balance -= amount;
            addTransaction(accNum, "WITHDRAWAL", amount, acc.balance);
        }
        fwrite(&acc, sizeof(Account), 1, temp);
    }
    
    fclose(fp);
    fclose(temp);
    
    remove(FILENAME);
    rename("temp.dat", FILENAME);
    
    printf("\n-----------------------------------------\n");
    printf("Withdrawal successful!\n");
    printf("New Balance: $%.2f\n", acc.balance);
}

void checkBalance() {
    int accNum;
    Account acc;
    
    system("clear || cls");
    displayHeader();
    printf("CHECK BALANCE\n");
    printf("-----------------------------------------\n");
    
    printf("Enter Account Number: ");
    if(scanf("%d", &accNum) != 1) {
        printf("\nInvalid account number!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    if(!findAccount(accNum, &acc)) {
        printf("\nAccount not found!\n");
        return;
    }
    
    printf("\n-----------------------------------------\n");
    printf("ACCOUNT DETAILS\n");
    printf("-----------------------------------------\n");
    printf("Account Number  : %d\n", acc.accountNumber);
    printf("Account Holder  : %s\n", acc.name);
    printf("Account Type    : %s\n", acc.accountType);
    printf("Current Balance : $%.2f\n", acc.balance);
    printf("Date Created    : %s\n", acc.dateCreated);
    printf("-----------------------------------------\n");
}

void viewAllAccounts() {
    Account acc;
    FILE *fp;
    int count = 0;
    
    system("clear || cls");
    displayHeader();
    printf("ALL ACCOUNTS\n");
    printf("-----------------------------------------\n");
    
    fp = fopen(FILENAME, "rb");
    if(fp == NULL) {
        printf("No accounts found!\n");
        return;
    }
    
    printf("%-10s %-25s %-15s %-15s\n", "Acc No.", "Name", "Type", "Balance");
    printf("-----------------------------------------\n");
    
    while(fread(&acc, sizeof(Account), 1, fp)) {
        printf("%-10d %-25s %-15s $%-14.2f\n", 
               acc.accountNumber, acc.name, acc.accountType, acc.balance);
        count++;
    }
    
    fclose(fp);
    
    printf("-----------------------------------------\n");
    printf("Total Accounts: %d\n", count);
}

void searchByName() {
    char searchName[MAX_NAME];
    Account acc;
    FILE *fp;
    int found = 0;
    
    system("clear || cls");
    displayHeader();
    printf("SEARCH ACCOUNT BY NAME\n");
    printf("-----------------------------------------\n");
    
    printf("Enter name to search: ");
    fgets(searchName, MAX_NAME, stdin);
    searchName[strcspn(searchName, "\n")] = 0;
    
    // Convert to lowercase for case-insensitive search
    for(int i = 0; searchName[i]; i++) {
        searchName[i] = tolower(searchName[i]);
    }
    
    fp = fopen(FILENAME, "rb");
    if(fp == NULL) {
        printf("\nNo accounts found!\n");
        return;
    }
    
    printf("\nSearch Results:\n");
    printf("-----------------------------------------\n");
    printf("%-10s %-25s %-15s %-15s\n", "Acc No.", "Name", "Type", "Balance");
    printf("-----------------------------------------\n");
    
    while(fread(&acc, sizeof(Account), 1, fp)) {
        char tempName[MAX_NAME];
        strcpy(tempName, acc.name);
        
        // Convert to lowercase for comparison
        for(int i = 0; tempName[i]; i++) {
            tempName[i] = tolower(tempName[i]);
        }
        
        // Check if search name is contained in account name
        if(strstr(tempName, searchName) != NULL) {
            printf("%-10d %-25s %-15s $%-14.2f\n", 
                   acc.accountNumber, acc.name, acc.accountType, acc.balance);
            found++;
        }
    }
    
    fclose(fp);
    
    if(found == 0) {
        printf("No accounts found matching '%s'\n", searchName);
    } else {
        printf("-----------------------------------------\n");
        printf("Total Matches: %d\n", found);
    }
}

void viewTransactionHistory() {
    int accNum;
    Transaction trans;
    FILE *fp;
    int count = 0;
    int choice;
    
    system("clear || cls");
    displayHeader();
    printf("TRANSACTION HISTORY\n");
    printf("-----------------------------------------\n");
    printf("1. View history for specific account\n");
    printf("2. View all transactions\n");
    printf("\nEnter choice: ");
    
    if(scanf("%d", &choice) != 1) {
        printf("\nInvalid choice!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    if(choice == 1) {
        printf("\nEnter Account Number: ");
        if(scanf("%d", &accNum) != 1) {
            printf("\nInvalid account number!\n");
            clearInputBuffer();
            return;
        }
        clearInputBuffer();
        
        Account acc;
        if(!findAccount(accNum, &acc)) {
            printf("\nAccount not found!\n");
            return;
        }
        
        printf("\nTransaction History for Account: %d (%s)\n", accNum, acc.name);
    } else if(choice == 2) {
        printf("\nAll Transactions:\n");
        accNum = -1; // Flag to show all
    } else {
        printf("\nInvalid choice!\n");
        return;
    }
    
    fp = fopen(TRANSACTION_FILE, "rb");
    if(fp == NULL) {
        printf("\nNo transaction history found!\n");
        return;
    }
    
    printf("-----------------------------------------\n");
    printf("%-10s %-18s %-12s %-15s %-12s %-10s\n", 
           "Acc No.", "Type", "Amount", "Balance After", "Date", "Time");
    printf("-----------------------------------------\n");
    
    while(fread(&trans, sizeof(Transaction), 1, fp)) {
        if(accNum == -1 || trans.accountNumber == accNum) {
            printf("%-10d %-18s $%-11.2f $%-14.2f %-12s %-10s\n",
                   trans.accountNumber, trans.type, trans.amount, 
                   trans.balanceAfter, trans.date, trans.time);
            count++;
        }
    }
    
    fclose(fp);
    
    printf("-----------------------------------------\n");
    printf("Total Transactions: %d\n", count);
    
    if(count == 0) {
        printf("No transactions found.\n");
    }
}

void modifyAccount() {
    int accNum;
    Account acc;
    FILE *fp, *temp;
    
    system("clear || cls");
    displayHeader();
    printf("MODIFY ACCOUNT\n");
    printf("-----------------------------------------\n");
    
    printf("Enter Account Number: ");
    if(scanf("%d", &accNum) != 1) {
        printf("\nInvalid account number!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    if(!findAccount(accNum, &acc)) {
        printf("\nAccount not found!\n");
        return;
    }
    
    printf("\nCurrent Details:\n");
    printf("Name: %s\n", acc.name);
    printf("Type: %s\n", acc.accountType);
    
    printf("\nEnter New Name (or press Enter to keep current): ");
    char newName[MAX_NAME];
    fgets(newName, MAX_NAME, stdin);
    newName[strcspn(newName, "\n")] = 0;
    if(strlen(newName) > 0) {
        strcpy(acc.name, newName);
    }
    
    printf("Enter New Account Type (or press Enter to keep current): ");
    char newType[20];
    fgets(newType, 20, stdin);
    newType[strcspn(newType, "\n")] = 0;
    if(strlen(newType) > 0) {
        strcpy(acc.accountType, newType);
    }
    
    // Update file
    fp = fopen(FILENAME, "rb");
    temp = fopen("temp.dat", "wb");
    
    if(fp == NULL || temp == NULL) {
        printf("\nError opening file!\n");
        if(fp) fclose(fp);
        if(temp) fclose(temp);
        return;
    }
    
    Account tempAcc;
    while(fread(&tempAcc, sizeof(Account), 1, fp)) {
        if(tempAcc.accountNumber == accNum) {
            fwrite(&acc, sizeof(Account), 1, temp);
        } else {
            fwrite(&tempAcc, sizeof(Account), 1, temp);
        }
    }
    
    fclose(fp);
    fclose(temp);
    
    remove(FILENAME);
    rename("temp.dat", FILENAME);
    
    printf("\n-----------------------------------------\n");
    printf("Account modified successfully!\n");
}

void deleteAccount() {
    int accNum;
    Account acc;
    FILE *fp, *temp;
    char confirm;
    
    system("clear || cls");
    displayHeader();
    printf("DELETE ACCOUNT\n");
    printf("-----------------------------------------\n");
    
    printf("Enter Account Number: ");
    if(scanf("%d", &accNum) != 1) {
        printf("\nInvalid account number!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    if(!findAccount(accNum, &acc)) {
        printf("\nAccount not found!\n");
        return;
    }
    
    printf("\nAccount Details:\n");
    printf("Name: %s\n", acc.name);
    printf("Balance: $%.2f\n", acc.balance);
    
    printf("\nAre you sure you want to delete this account? (y/n): ");
    scanf("%c", &confirm);
    clearInputBuffer();
    
    if(confirm != 'y' && confirm != 'Y') {
        printf("\nAccount deletion cancelled.\n");
        return;
    }
    
    fp = fopen(FILENAME, "rb");
    temp = fopen("temp.dat", "wb");
    
    if(fp == NULL || temp == NULL) {
        printf("\nError opening file!\n");
        if(fp) fclose(fp);
        if(temp) fclose(temp);
        return;
    }
    
    while(fread(&acc, sizeof(Account), 1, fp)) {
        if(acc.accountNumber != accNum) {
            fwrite(&acc, sizeof(Account), 1, temp);
        }
    }
    
    fclose(fp);
    fclose(temp);
    
    remove(FILENAME);
    rename("temp.dat", FILENAME);
    
    printf("\n-----------------------------------------\n");
    printf("Account deleted successfully!\n");
}