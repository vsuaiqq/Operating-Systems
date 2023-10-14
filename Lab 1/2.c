#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>

#define START_CAPACITY 32
#define MAX_LOGIN_LENGTH 6
#define MIN_PIN_CODE_VALUE 0
#define MAX_PIN_CODE_VALUE 100000

typedef enum read_line_status_code
{
    rl_success,
    rl_alloc_err,
} read_line_status_code;

typedef enum register_user_status_code
{
    ru_success,
    ru_alloc_err,
    ru_already_registered,
} register_user_status_code;

typedef enum login_user_status_code
{
    lu_success,
    lu_not_found,
    lu_not_valid,
} login_user_status_code;

typedef enum put_sanctions_status_code 
{
    ps_success,
    ps_not_found,
} put_sanctions_status_code;

typedef enum get_elapsed_time_status_code
{
    get_success,
    get_negative_time,
    get_invalid_parameter,
} get_elapsed_time_status_code;

typedef enum authorized_commands
{
    TIME,
    DATE,
    HOWMUCH,
    LOGOUT,
    SANCTIONS,
    AU_STOP,
    AU_UNKNOWN,
} authorized_commands;

typedef enum non_authorized_commands
{
    LOGIN,
    REGISTER,
    NON_AU_STOP,
    NON_AU_UNKNOWN,
} non_authorized_commands;

typedef struct User
{
    char login[MAX_LOGIN_LENGTH + 1];
    int pin_code;
    int sanctions;
} User;

authorized_commands auth_input_handler(char* input_str) 
{
    if (!strcmp(input_str, "time")) return TIME;
    else if (!strcmp(input_str, "date")) return DATE;
    else if (!strcmp(input_str, "howmuch")) return HOWMUCH;
    else if (!strcmp(input_str, "logout")) return LOGOUT;
    else if (!strcmp(input_str, "sanctions")) return SANCTIONS;
    else if (!strcmp(input_str, "stop")) return AU_STOP;
    else return AU_UNKNOWN;
}

non_authorized_commands non_auth_input_handler(char* input_str) 
{
    if (!strcmp(input_str, "login")) return LOGIN;
    else if (!strcmp(input_str, "register")) return REGISTER;
    else if (!strcmp(input_str, "stop")) return NON_AU_STOP;
    else return NON_AU_UNKNOWN;
}
 
void multiple_free(int args_num, ...) 
{
    va_list ptr;
    va_start(ptr, args_num);
    for (int i = 0; i < args_num; ++i) 
    {
        void* ptr_to_free = va_arg(ptr, void*);
        if (ptr_to_free != NULL) free(ptr_to_free);
    }
    va_end(ptr);
}

bool validate_pin_code(int pin_code) 
{
    return pin_code >= 0 && pin_code <= 100000;
}

bool validate_login(char* login) 
{
    return strlen(login) <= 6;
}

bool validate_flag(char* flag)
{
    return (strcmp(flag, "-s") == 0 || strcmp(flag, "-m") == 0 || strcmp(flag, "-h") == 0 || strcmp(flag, "-y") == 0);
}

bool confirm_sanctions(int code) 
{
    return code == 12345;
}

bool find_user(User* users, char* login, int users_len) 
{
    for (int i = 0; i < users_len; ++i) 
    {
        if (!strcmp(users[i].login, login)) return true;
    }
    return false;
}

read_line_status_code read_line(char** buff_str) 
{
    *buff_str = (char*)malloc(sizeof(char) * (MAX_LOGIN_LENGTH + 1));
    if (*buff_str == NULL) return rl_alloc_err;

    int cur_capacity = MAX_LOGIN_LENGTH + 1, cur_size = 0;
    char c = getchar();

    while (isspace(c)) c = getchar();

    while (!isspace(c)) 
    {
        ++cur_size;
        if (cur_size > MAX_LOGIN_LENGTH) 
        {
            cur_capacity *= 2;
            char* tmp = (char*)realloc(*buff_str, cur_capacity);
            if (tmp == NULL) 
            {
                free(*buff_str);
                return rl_alloc_err;
            }
            *buff_str = tmp;
        }
        (*buff_str)[cur_size - 1] = c;
        c = getchar();
    }
    (*buff_str)[cur_size] = '\0';

    return rl_success;
}

register_user_status_code register_user(User** users, char* login, int pin_code, int* capacity, int* size) 
{
    if (find_user(*users, login, *size)) return ru_already_registered;
    if (size + 1 > capacity) 
    {
        *capacity *= 2;
        User* tmp = (User*)realloc(*users, *capacity);
        if (tmp == NULL) 
        {
            free(*users);
            return ru_alloc_err;
        }
    }
    strcpy((*users)[*size].login, login);
    (*users)[*size].pin_code = pin_code;
    (*users)[*size].sanctions = -1;
    ++(*size);
    return ru_success;
}

login_user_status_code login_user(User* users, char* login, int pin_code, int size, 
            bool* is_logged, int* requests_lim, char cur_user[MAX_LOGIN_LENGTH + 1]) 
{
    for (int i = 0; i < size; ++i) 
    {
        if (!strcmp(users[i].login, login)) 
        {
            if (users[i].pin_code == pin_code) 
            {
                *is_logged = true, *requests_lim = users[i].sanctions, strcpy(cur_user, login);
                return lu_success;
            }
            return lu_not_valid;
        }
    }
    return lu_not_found;
}

put_sanctions_status_code put_sanctions(User* users, char* username, int number, int size, 
            int* req_lim, char cur_user[MAX_LOGIN_LENGTH + 1]) 
{
    for (int i = 0; i < size; ++i) 
    {
        if (!strcmp(users[i].login, username)) 
        {
            users[i].sanctions = number;
            if (!strcmp(users[i].login, cur_user)) *req_lim = number;
            return ps_success;
        }
    }
    return ps_not_found;
}

get_elapsed_time_status_code get_elapsed_time(char *time_str, char flag, double* res) {
    int d, m, y;
    if (sscanf(time_str, "%d/%d/%d", &d, &m, &y) != 3) return get_invalid_parameter;

    struct tm tm_start;
    tm_start.tm_year = y - 1900;
    tm_start.tm_mon = m - 1;
    tm_start.tm_mday = d;
    tm_start.tm_hour = 0;
    tm_start.tm_min = 0;
    tm_start.tm_sec = 0;
    time_t start_time_seconds = mktime(&tm_start);
    time_t finish_time_seconds = time(NULL);

    double diff_seconds = difftime(finish_time_seconds, start_time_seconds);
    if (diff_seconds < 0) return get_negative_time;
    switch (flag)
    {
        case 's':
            *res = diff_seconds;
            break;
        case 'm':
            *res = diff_seconds / 60.0;
            break;
        case 'h':
            *res = diff_seconds / 3600.0;
            break;
        case 'y':
            *res = diff_seconds / 31536000.0;
    }
    return get_success;
}

void get_current_time(char* str_time)
{
	time_t cur_time = time(NULL);
	struct tm *cur_tm = localtime(&cur_time);
	sprintf(str_time, "%d:%d:%d", cur_tm->tm_hour, cur_tm->tm_min, cur_tm->tm_sec);
}

void get_current_date(char* str_date)
{
	time_t cur_time = time(NULL);
	struct tm *cur_tm = localtime(&cur_time);
	char day[3], month[3];
	if (cur_tm->tm_mday < 10) sprintf(day, "0%d", cur_tm->tm_mday);
	else sprintf(day, "%d", cur_tm->tm_mday);
	if (cur_tm->tm_mon < 9) sprintf(month, "0%d", cur_tm->tm_mon + 1);
	else sprintf(month, "%d", cur_tm->tm_mon + 1);
	sprintf(str_date, "%s.%s.%d", day, month, cur_tm->tm_year + 1900);
}

bool check_req_lim(int req_lim, int req_cnt) 
{

    return (req_lim == -1) ? true : (req_cnt <= req_lim);
}
 
void print_start_menu() 
{
    printf("print \"login\" to log into your account\n");
    printf("print \"register\" to register account\n");
    printf("print \"stop\" to finish the programm\n");
}

void print_commands_menu() 
{
    printf("print \"time\" to get current time\n");
    printf("print \"date\" to get current date\n");
    printf("print \"howmuch <time> flag\" to count the time elapsed since a certain date in time parameter\n");
    printf("print \"logout\" to exit from account\n");
    printf("print \"sanctions username <number>\" to limit username requests to number\n");
    printf("print \"stop\" to finish the programm\n");
}

int main() 
{
    char* input_str;
    char* login;
    char cur_user[MAX_LOGIN_LENGTH + 1];
    bool is_logged = false;
    int users_capacity = START_CAPACITY, users_size = 0, requests_cnt, requests_lim, pin_code;

    User* all_users = (User*)malloc(sizeof(User) * users_capacity);
    if (all_users == NULL) 
    {
        printf("allocate error\n");
        return 1;
    }

    print_start_menu();

    if (read_line(&input_str) == rl_alloc_err)
    {
        multiple_free(1, all_users);
        printf("allocate error\n");
        return 1;
    }

    while (strcmp(input_str, "stop"))
    {
        if (is_logged) 
        {
            switch (auth_input_handler(input_str))
            {

                case TIME:
                    ++requests_cnt;
                    if (check_req_lim(requests_lim, requests_cnt)) 
                    {
                        char time_str[9];
                        get_current_time(time_str);
                        printf("time: %s\n", time_str);
                    }
                    print_commands_menu();
                    break;

                case DATE:
                    ++requests_cnt;
                    if (check_req_lim(requests_lim, requests_cnt)) 
                    {
                        char date_str[11];
                        get_current_date(date_str);
                        printf("date: %s\n", date_str);
                    }
                    print_commands_menu();
                    break;

                case HOWMUCH:
                    ++requests_cnt;
                    if (check_req_lim(requests_lim, requests_cnt)) 
                    {
                        char* time;
                        char* flag;
                        double res;
                        printf("time format: <day>/<month>/<year>\n");
                        if (read_line(&time) == rl_alloc_err)
                        {
                            printf("allocate error\n");
                            multiple_free(2, input_str, all_users);
                            return 1;
                        }
                        printf("available flags: \"-s\" \"-m\" \"-h\" \"-y\"\n");
                        do 
                        {
                            if (read_line(&flag) == rl_alloc_err)
                            {
                                printf("allocate error\n");
                                multiple_free(3, input_str, all_users, time);
                                return 1;
                            }
                        } while (!validate_flag(flag));
                        switch (get_elapsed_time(time, flag[1], &res))
                        {
                        case get_success:
                            printf("res: %f\n", res);
                            break;
                        case get_negative_time:
                            printf("this date has not yet occurred\n");
                            break;
                        case get_invalid_parameter:
                            printf("invalid parameter\n");
                        }
                        multiple_free(2, time, flag);
                    }
                    print_commands_menu();
                    break;

                case LOGOUT:
                    is_logged = false;
                    print_start_menu();
                    break;

                case SANCTIONS:
                    ++requests_cnt;
                    if (check_req_lim(requests_lim, requests_cnt)) 
                    {
                        char* username;
                        int confirm_code, number;
                        if (read_line(&username) == rl_alloc_err)
                        {
                            printf("allocate error\n");
                            multiple_free(2, input_str, all_users);
                            return 1;
                        }
                        printf("number must be non-negative\n");
                        int k = 0;
                        do 
                        {
                            k = scanf("%d", &number);
                        } while (number < 0 && k != 1);
                        printf("12345 to confirm\n");
                        scanf("%d", &confirm_code);
                        if (confirm_sanctions(confirm_code)) 
                        {
                            switch (put_sanctions(all_users, username, number, users_size, &requests_lim, cur_user))
                            {
                                case ps_success:
                                    break;
                                case ps_not_found:
                                    printf("user doesnt found\n");
                            }
                        }
                        free(username);
                    }
                    print_commands_menu();
                    break;

                case AU_STOP:
                case AU_UNKNOWN:
                    break;

            }
        }
        else 
        {
            switch (non_auth_input_handler(input_str))
            {

                case LOGIN:
                    printf("login length should not exceed 6 characters\n");
                    do 
                    {
                        if (read_line(&login) == rl_alloc_err)
                        {
                            printf("allocate error\n");
                            multiple_free(2, input_str, all_users);
                            return 1;
                        }
                    } while (!validate_login(login));
                    printf("pin code must have int type\n");
                    printf("pin code must be in range [0, 100000]\n");
                    do 
                    {
                        scanf("%d", &pin_code);
                    } while (!validate_pin_code(pin_code));
                    switch (login_user(all_users, login, pin_code, users_size, &is_logged, &requests_lim, cur_user))
                    {
                        case lu_success:
                            print_commands_menu();
                            break;
                        case lu_not_valid:
                            printf("pincode is not valid\n");
                            print_start_menu();
                            break;
                        case lu_not_found:
                            printf("user doesnt found\n");
                            print_start_menu();
                    }
                    requests_cnt = 0;
                    free(login);
                    break;

                case REGISTER:
                    printf("login length should not exceed 6 characters\n");
                    do 
                    {
                        if (read_line(&login) == rl_alloc_err)
                        {
                            printf("allocate error\n");
                            multiple_free(2, input_str, all_users);
                            return 1;
                        }
                    } while (!validate_login(login));
                    printf("pin code must have int type\n");
                    printf("pin code must be in range [0, 100000]\n");
                    do 
                    {
                        scanf("%d", &pin_code);
                    } while (!validate_pin_code(pin_code));
                    switch (register_user(&all_users, login, pin_code, &users_capacity, &users_size))
                    {
                        case ru_success:
                            print_start_menu();
                            break;
                        case ru_alloc_err:
                            printf("allocate error\n");
                            multiple_free(2, login, input_str);
                            return 1;
                        case ru_already_registered:
                            printf("user already registered\n");
                            print_start_menu();
                    }
                    multiple_free(1, login);
                    break;

                case NON_AU_STOP:
                case NON_AU_UNKNOWN:
                    break;

            }
        }
        
        free(input_str);
        if (read_line(&input_str) == rl_alloc_err)
        {
            printf("allocate error\n");
            free(all_users);
            return 1;
        }
    }
    
    multiple_free(2, input_str, all_users);

    return 0;
}