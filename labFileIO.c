#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <crypt.h>

struct user {
    char username[50];
    char password[256];
    char firstname[50];
    char lastname[50];
    int admin;
};

char* cs221Hash(const char* password) {
    return crypt(password, "00");
}

struct user* createUsers(int* count) {
    FILE *file = fopen("credential_file.txt", "r");
    if (!file) return NULL;

    char line[1024];
    *count = 0;
    while (fgets(line, sizeof(line), file)) {
        (*count)++;
    }

    fseek(file, 0, SEEK_SET);
    struct user *users = malloc(*count * sizeof(struct user));
    fclose(file);
    return users;
}

void populateUsers(void* users) {
    FILE *file = fopen("credential_file.txt", "r");
    if (!file) return;

    struct user *u = (struct user *)users;
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%s\t%s\t%s\t%s\t%d", 
               u->firstname, u->lastname, u->username, u->password, &u->admin);
        u++;
    }
    fclose(file);
}

int checkAdminPassword(char* password, struct user* users, int count) {
    char *hashedPassword = cs221Hash(password);
    struct user *current_user = users;
    for (int i = 0; i < count; ++i, ++current_user) {
        if (strcmp(current_user->username, "admin") == 0) {
            if (strcmp(current_user->password, hashedPassword) == 0) {
                return 1;
            } else {
                return 0;
            }
        }
    }
    return 0;
}

struct user* addUser(struct user* users, int* count, char* username, char* password, char* firstname, char* lastname, int admin) {
    struct user *users_expanded = realloc(users, (*count + 1) * sizeof(struct user));
    if (!users_expanded) return NULL;
    struct user *new_user = users_expanded + *count;
    strcpy(new_user->username, username);
    strcpy(new_user->password, cs221Hash(password));
    strcpy(new_user->firstname, firstname);
    strcpy(new_user->lastname, lastname);
    new_user->admin = admin;
    (*count)++;
    return users_expanded;
}

void saveUsers(struct user* users, int count) {
    FILE *file = fopen("credential_file.txt", "w");
    if (!file) return;

    struct user *current_user = users;
    for (int i = 0; i < count; i++, current_user++) {
        fprintf(file, "%s\t%s\t%s\t%s\t%d\n",
                current_user->firstname, current_user->lastname,
                current_user->username, current_user->password,
                current_user->admin);
    }
    fclose(file);
}

int main(void) {
    int user_count = 0;
    struct user* users = createUsers(&user_count);
    if (users == NULL) {
        return EXIT_FAILURE;
    }
    populateUsers(users);

    printf("Enter admin password to add new users:");
    char entered_admin_password[50];
    scanf("%s", entered_admin_password);
    if (checkAdminPassword(entered_admin_password, users, user_count)) {
        struct user new_user;
        printf("Enter username:");
        scanf("%s", new_user.username);
        printf("Enter first name:");
        scanf("%s", new_user.firstname);
        printf("Enter last name:");
        scanf("%s", new_user.lastname);
        printf("Enter password:");
        scanf("%s", new_user.password);
        printf("Enter admin level:");
        scanf("%d", &(new_user.admin));
        users = addUser(users, &user_count, new_user.username, new_user.password, new_user.firstname, new_user.lastname, new_user.admin);
        if (users == NULL) {
            return EXIT_FAILURE;
        }
    }
    saveUsers(users, user_count);
    free(users);
    return EXIT_SUCCESS;
}
