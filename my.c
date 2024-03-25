#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdlib.h>

int main() {
    // Получаем идентификатор пользователя и группы
    uid_t userId = getuid();
    gid_t groupId = getgid();
    
    // Получаем информацию о пользователе
    struct passwd *userInfo = getpwuid(userId);
    
    // Получаем информацию о группах
    int groupCount = 0;
    gid_t *groups = NULL;
    
    groupCount = getgroups(0, NULL); // Сначала узнаем, сколько групп у пользователя
    if (groupCount > 0) {
        groups = (gid_t *)malloc(groupCount * sizeof(gid_t));
        getgroups(groupCount, groups); // Затем получаем список групп
    }
    
    // Выводим информацию о пользователе
    printf("User ID: %d\n", userId);
    printf("User Name: %s\n", userInfo->pw_name);
    
    // Выводим информацию о группах
    printf("Group ID: %d\n", groupId);
    printf("Group Name: %s\n", getgrgid(groupId)->gr_name);
    
    printf("Groups:");
    for (int i = 0; i < groupCount; i++) {
        printf(" %s", getgrgid(groups[i])->gr_name);
    }
    printf("\n");
    
    // Освобождаем выделенную память
    if (groups != NULL) {
        free(groups);
    }
    
    return 0;
}

