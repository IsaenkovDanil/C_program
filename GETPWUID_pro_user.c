#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <regex>

void myid(const char *id) {
	uid_t uid;
	std::string s(id);
	struct passwd *userInfo = NULL;

	std::regex reg("^[0-9]+$");
	if (std::regex_match(id, reg)) {
		uid = std::stoi(id);
		userInfo = getpwuid(uid);
	} else {
		userInfo = getpwnam(id);
	}

	if (!userInfo) {
		printf("id: \"%s\":no such user\n", id);
		perror("err:");
		return;
	}

	printf("uid=%d(%s) ", (int)uid, userInfo->pw_name);

	struct group *groupInfo = getgrgid(userInfo->pw_gid);

	if (groupInfo) {
		printf("gid=%d(%s) ", (int)userInfo->pw_gid, groupInfo->gr_name);
	} else {
		printf("gid=%d ", (int)userInfo->pw_gid);
	}

	int size = 0;
	gid_t *list = NULL;

	if (getgrouplist(userInfo->pw_name, userInfo->pw_gid, list, &size) == -1) {
		list = (gid_t*)calloc(size, sizeof(gid_t));
		getgrouplist(userInfo->pw_name, userInfo->pw_gid, list, &size);
	}

	printf("groups=");
	for (int i = 0; i < size; i++) {
		struct group *groupInfo = getgrgid(list[i]);
		if (groupInfo) {
			printf("%d(%s)", (int)list[i], groupInfo->gr_name);
		} else {
			printf("%d", (int)list[i]);
		}

		if (i < size - 1) {
			printf(",");
		}
	}
	printf("\n");
	free(list);
}

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("enter user id\n");
		exit(1);
	} else if (argc > 2) {
		printf("id: extra operand \'%s\'\n", argv[2]);
		exit(1);
	}

	myid(argv[1]);
}
