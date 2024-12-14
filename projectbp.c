#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define RESET "\033[0m"
#define GREEN "\033[92m"
#define ORANGE "\033[38;5;214m"
#define BLUE "\033[94m"
#define CYAN "\033[96m"
#define RED "\033[1;31m"

#define ITEMS 10
#define NAME_LEN 50
#define PHONE_LEN 15
#define PASSWORD_LEN 20
#define ITEM_NAME_LEN 30

struct Customer
{
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    char password[PASSWORD_LEN];
};

struct FoodItem
{
    char name[ITEM_NAME_LEN];
    int price;
};

void show_loading_animation()
{
    printf(BLUE "\nProcessing");
    for (int i = 0; i < 2; i++)
    {
        printf(" _");
        fflush(stdout);
        sleep(1);
    }
    printf(" !\n" RESET);
}

void get_current_date(char *date)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date, "%02d/%02d/%02d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year % 100);
}

int calculate_total_amount(const char *cart_file)
{
    FILE *file = fopen(cart_file, "r");

    int total_amount = 0;
    char food_item[100];
    int price;

    while (fscanf(file, "%99[^,],%d\n", food_item, &price) != EOF)
    {
        total_amount += price;
    }

    fclose(file);
    return total_amount;
}

void save_to_file(struct Customer customer, char *file_path)
{
    FILE *file = fopen(file_path, "a");
    fprintf(file, "%s,%s,%s\n", customer.name, customer.phone, customer.password);
    fclose(file);
}

int check_login(char *phone, char *password, char *file_path)
{
    FILE *file = fopen(file_path, "r");
    char file_phone[PHONE_LEN], file_password[PASSWORD_LEN], file_name[NAME_LEN];
    int found = 0;

    while (fscanf(file, "%49[^,],%14[^,],%19[^,\n]\n", file_name, file_phone, file_password) != EOF)
    {
        if (strcmp(phone, file_phone) == 0 && strcmp(password, file_password) == 0)
        {
            show_loading_animation();
            printf(GREEN "\nLogin Successful! Welcome, %s!\n" RESET, file_name);
            printf(GREEN "\n" RESET);
            found = 1;
            break;
        }
    }

    if (found == 0)
    {
        show_loading_animation();
        printf(GREEN "Login Failed! Incorrect Phone or Password.\n" RESET);
    }
    fclose(file);

    return found;
}

void display_menu(struct FoodItem menu[], int item_count)
{
    printf(ORANGE "\n|-------------------- MENU --------------------|" RESET);
    printf("\n");
    printf("S.No | Food Item                 | Price (BDT)\n");
    printf("-----------------------------------------------\n");
    for (int i = 0; i < item_count; i++)
    {
        printf("%-5d| %-25s | %-11d\n", i + 1, menu[i].name, menu[i].price);
        printf(GREEN "-----------------------------------------------\n" RESET);
    }
    printf(ORANGE "-----------------------------------------------\n" RESET);
}

void add_food_item(struct FoodItem menu[], int *item_count)
{
    if (*item_count >= ITEMS)
    {
        show_loading_animation();
        printf(GREEN "Menu is full! Cannot add more items.\n" RESET);
        return;
    }

    struct FoodItem new_item;
    printf(ORANGE "\nEnter Food Name: " RESET);
    scanf(" %[^\n]s", new_item.name);

    printf(ORANGE "Enter Food Price (BDT): " RESET);
    scanf("%d", &new_item.price);

    menu[*item_count] = new_item;
    (*item_count)++;
    show_loading_animation();
    printf(GREEN "\nFOOD ITEM ADDED SUCCESSFULLY !\n" RESET);
}

void delete_food_item(struct FoodItem menu[], int *item_count)
{
    if (*item_count == 0)
    {
        show_loading_animation();
        printf(GREEN "No food items to delete.\n" RESET);
        return;
    }

    int index;
    printf(ORANGE "Enter the number of the food item to delete (1 to %d): " RESET, *item_count);
    scanf("%d", &index);

    if (index < 1 || index > *item_count)
    {
        printf(GREEN "\nInvalid item number.\n" RESET);
        return;
    }

    for (int i = index - 1; i < *item_count - 1; i++)
    {
        menu[i] = menu[i + 1];
    }
    (*item_count)--;
    show_loading_animation();
    printf(GREEN "FOOD ITEM DELETED SUCCESSFULLY !\n" RESET);
}

void view_food_items(struct FoodItem menu[], int item_count)
{
    if (item_count == 0)
    {
        printf(GREEN "No food items available.\n" RESET);
    }
    else
    {
        display_menu(menu, item_count);
    }
}

void add_to_cart(struct FoodItem menu[], int item_count, char *cart_file)
{
    int order_choice;
    FILE *file = fopen(cart_file, "a");
    printf(ORANGE "\nSelect food items to add to cart (Enter 0 to stop)\n" RESET);

    while (1)
    {
        printf("\nEnter the item number (1 to %d, or 0 to stop): ", item_count);
        scanf("%d", &order_choice);

        if (order_choice == 0)
        {
            break;
        }
        else if (order_choice > 0 && order_choice <= item_count)
        {
            fprintf(file, "%s,%d\n", menu[order_choice - 1].name, menu[order_choice - 1].price);
            printf(CYAN "%s added to cart.\n" RESET, menu[order_choice - 1].name);
            printf("_");
        }
        else
        {
            printf(GREEN "Invalid choice! Please try again.\n" RESET);
        }
    }
    fclose(file);
}

void view_cart(char *cart_file)
{
    FILE *file = fopen(cart_file, "r");
    char item_name[ITEM_NAME_LEN];
    int item_price;
    int total_amount = 0;

    printf(CYAN "\n|------------------ Your Cart ------------------|\n" RESET);
    printf("\n%-30s | %-10s\n", "Item Name", "Price (BDT)");
    printf("----------------------------------------------\n");

    while (fscanf(file, "%29[^,],%d\n", item_name, &item_price) != EOF)
    {
        printf("%-30s | BDT %-8d\n", item_name, item_price);
        total_amount += item_price;
    }

    printf(CYAN "----------------------------------------------\n" RESET);
    printf(ORANGE "Total Amount: BDT %d\n" RESET, total_amount);
    fclose(file);
}

void view_sell_history(const char *sell_history_file)
{
    FILE *file = fopen(sell_history_file, "r");

    char date[9], customer_name[100], address[200];
    int amount;

    show_loading_animation();
    printf(ORANGE "\n|---------------------- SELL HISTORY ----------------------|\n" RESET);
    printf(CYAN "Date       | Customer Name        |  Amount  |   Address\n" RESET);
    printf("-----------------------------------------------------------\n");

    while (fscanf(file, "%8[^,],%99[^,],%d,%199[^\n]\n", date, customer_name, &amount, address) != EOF)
    {
        printf("%-10s | %-20s | BDT %-4d | %s\n", date, customer_name, amount, address);
    }
    printf(ORANGE "-----------------------------------------------------------\n" RESET);

    fclose(file);
}

void place_order_and_pay(char *cart_file, const char *sell_history_file)
{
    int error = 0;
    FILE *file = fopen(cart_file, "r");
    if (file == NULL || fgetc(file) == EOF)
    {
        printf(GREEN "YOUR CART IS EMPTY NOW - PLEASE ADD SOMETHING TO YOUR CART!\n" RESET);
        fclose(file);
        return;
    }
    view_cart(cart_file);

    printf(CYAN "\nPlace your order? (YES=1 or NO=0): " RESET);
    int place_order;
    scanf("%d", &place_order);

    if (place_order == 1)
    {
        char customer_name[100];
        char customer_address[200];

        printf(ORANGE "Enter your name: " RESET);
        scanf(" %[^\n]", customer_name);

        printf(ORANGE "Enter your address: " RESET);
        scanf(" %[^\n]", customer_address);

        double total_amount = calculate_total_amount(cart_file);

        show_loading_animation();
        printf(GREEN "\nOrder placed successfully!\n" RESET);

        printf(ORANGE "\nPayment Options" RESET);
        printf(ORANGE "\n_______________ \n" RESET);
        printf(CYAN "1. Bkash\n" RESET);
        printf(CYAN "2. Nagad\n" RESET);
        printf(ORANGE "Select payment method (1 or 2): " RESET);
        int payment_choice;
        scanf("%d", &payment_choice);

        if (payment_choice == 1)
        {
            show_loading_animation();
            printf(GREEN "\nYou selected Bkash. >> Please complete the payment via your Bkash app.\nTHANK YOU!" RESET);
        }
        else if (payment_choice == 2)
        {
            show_loading_animation();
            printf(GREEN "\nYou selected Nagad. >> Please complete the payment via your Nagad app.\n" RESET);
        }
        else
        {
            show_loading_animation();
            printf(GREEN "Invalid choice! No payment selected.\n" RESET);
            error = 1;
        }

        if (error == 0)
        {
            char current_date[9];
            get_current_date(current_date);

            FILE *sell_history = fopen(sell_history_file, "a");
            if (sell_history != NULL)
            {
                fprintf(sell_history, "%s,%s,%d,%s\n", current_date, customer_name, (int)total_amount, customer_address);
                fclose(sell_history);
            }
            else
            {
                printf("Error saving order history.\n");
            }

            FILE *clear_file = fopen(cart_file, "w");
            if (clear_file == NULL)
            {
                printf("Error clearing the cart.\n");
            }
            else
            {
                printf(GREEN "\nYOUR CART IS NOW EMPTY.\n" RESET);
            }
            fclose(clear_file);
        }
        else
        {
            printf(GREEN "You can continue browsing the menu.\n" RESET);
        }
        fclose(file);
    }
    else
    {
        printf(GREEN "\nPLEASE TRY AGAIN!" RESET);
    }
}

int main()
{
    struct Customer customer;
    struct FoodItem menu[ITEMS] = {
        {"Burger", 150},
        {"Pizza", 300},
        {"Pasta", 200},
        {"Salad", 100},
        {"Sushi", 400},
        {"Sandwich", 120}};
    int item_count = 6;
    int choice, role, custop, wantlogin, menuopen = 0, adminOpen = 0;
    ;
    int admin_id, entered_admin_id = 1234;
    FILE *file1,*file2,*file3;
    file1 = fopen("C:\\Users\\TIMELORD\\capstone project\\Project.txt","w");
    file2 = fopen("C:\\Users\\TIMELORD\\capstone project\\cart_file.txt","w");
    file3 = fopen("C:\\Users\\TIMELORD\\capstone project\\sell_history.txt","w");
    

    char file_path[] = "C:\\Users\\TIMELORD\\capstone project\\Project.txt";
    char cart_file[] = "C:\\Users\\TIMELORD\\capstone project\\cart_file.txt";
    char sell_history_file[] = "C:\\Users\\TIMELORD\\capstone project\\sell_history.txt";

    while (1)
    {

        printf(GREEN "Welcome To Our Restaurant\n" RESET);
        printf(GREEN "-------------------------\n" RESET);
        printf(ORANGE "1: Admin Login\n" RESET);
        printf(ORANGE "2: Customer\n" RESET);
        printf(CYAN "\nSelect an option (1 or 2): " RESET);
        scanf("%d", &role);
        printf("----------------------------\n");

        if (role == 1)
        {
            while (1)
            {
                printf(GREEN "Enter Admin Password: " RESET);
                scanf("%d", &admin_id);

                if (admin_id == entered_admin_id)
                {
                    show_loading_animation();
                    printf(ORANGE "\nAdmin Login Successful!" RESET);

                    do
                    {
                        printf(ORANGE "\n------------------------" RESET);
                        printf("\n1: View Food Items\n2: Add Food Item\n3: Delete Food Item\n4: View Sell History \n5: Logout\n");
                        printf(CYAN "SELECT AN OPTION: " RESET);
                        scanf("%d", &choice);
                        if (choice == 1)
                        {
                            view_food_items(menu, item_count);
                            adminOpen = 1;
                        }
                        else if (choice == 2)
                        {
                            add_food_item(menu, &item_count);
                        }
                        else if (choice == 3)
                        {
                            if (adminOpen == 1)
                            {
                                delete_food_item(menu, &item_count);
                            }
                            else
                            {
                                printf(RED "\nYou Need To Open The Menu  First !\n" RESET);
                            }
                        }
                        else if (choice == 4)
                        {
                            view_sell_history(sell_history_file);
                        }
                        else if (choice == 5)
                        {
                            printf(GREEN "\nLogout Successfully !\n\n" RESET);
                            break;
                        }
                        else
                        {
                            printf(RED "\nInvalid choice. Please enter 1, 2, 3, or 4.\n" RESET);
                        }
                    } while (choice != 5);
                    break;
                }
                else
                {
                    printf(RED "\nIncorrect Admin Password (Please Try Again!)\n" RESET);
                }
            }
        }

        else if (role == 2)
        {
            printf(GREEN "1: Sign Up\n" RESET);
            printf(GREEN "2: Login\n" RESET);
            printf(CYAN "Select an option: " RESET);
            scanf("%d", &custop);

            if (custop == 1)
            {
                printf(ORANGE "Enter your Name: " RESET);
                scanf("%s", customer.name);
                printf(ORANGE "Enter your Phone: " RESET);
                scanf("%s", customer.phone);
                printf(ORANGE "Enter your Password: " RESET);
                scanf("%s", customer.password);
                save_to_file(customer, file_path);

                printf(CYAN "\nDo You Want to Login Now [yes = 1 / No = 0] : " RESET);
                scanf("%d", &wantlogin);
                if (wantlogin == 1)
                {

                    while (1)
                    {
                        printf(ORANGE "Enter your Phone: " RESET);
                        scanf("%s", customer.phone);
                        printf(ORANGE "Enter your Password: " RESET);
                        scanf("%s", customer.password);

                        if (check_login(customer.phone, customer.password, file_path))
                        {
                            do
                            {
                                printf(CYAN"-------------------"RESET);
                                printf("\n1: View Menu\n2: Add to Cart\n3: My Cart\n4: Logout\n");
                                printf(CYAN "SELECT AN OPTION: " RESET);
                                scanf("%d", &choice);

                                if (choice == 1)
                                {
                                    display_menu(menu, item_count);
                                    menuopen = 1;
                                }
                                else if (choice == 2)
                                {
                                    if (menuopen == 1)
                                    {
                                        add_to_cart(menu, item_count, cart_file);
                                    }
                                    else
                                    {
                                        printf(RED "\nSORRY YOU SHOULD OPEN THE MENU FIRST!\n_" RESET);
                                    }
                                }
                                else if (choice == 3)
                                {
                                    place_order_and_pay(cart_file, sell_history_file);
                                }
                                else if (choice == 4)
                                {
                                    printf(GREEN "Logout Successfully - Thank You For Dining With Us!\n\n" RESET);
                                    break;
                                }
                                else
                                {
                                    printf(RED "Invalid option! Please try again.\n" RESET);
                                }
                            } while (choice != 4);
                            break;
                        }
                        else
                        {
                            printf(RED "PLEASE TRY AGAIN -\n" RESET);
                        }
                    }
                }
                else
                {
                    printf(GREEN "Thank you! You can login later.\n" RESET);
                }
            }
            else if (custop == 2)
            {
                while (1)
                {
                    printf(ORANGE "Enter your Phone: " RESET);
                    scanf("%s", customer.phone);
                    printf(ORANGE "Enter your Password: " RESET);
                    scanf("%s", customer.password);

                    if (check_login(customer.phone, customer.password, file_path))
                    {
                        do
                        {
                            printf(CYAN"-------------------"RESET);
                            printf("\n1: View Menu\n2: Add to Cart\n3: My Cart\n4: Logout\n");
                            printf(CYAN "SELECT AN OPTION: " RESET);
                            scanf("%d", &choice);

                            if (choice == 1)
                            {
                                display_menu(menu, item_count);
                                menuopen = 1;
                            }
                            else if (choice == 2)
                            {
                                if (menuopen == 1)
                                {
                                    add_to_cart(menu, item_count, cart_file);
                                }
                                else
                                {
                                    printf(RED "\nSORRY YOU SHOULD OPEN THE MENU FIRST!\n" RESET);
                                }
                            }
                            else if (choice == 3)
                            {
                                place_order_and_pay(cart_file, sell_history_file);
                            }
                            else if (choice == 4)
                            {
                                printf(GREEN "Logout Successfully - Thank You For Dining With Us!\n\n" RESET);
                                break;
                            }
                            else
                            {
                                printf(RED "Invalid option! Please try again.\n" RESET);
                            }
                        } while (choice != 4);
                        break;
                    }
                    else
                    {
                        printf(RED "PLEASE TRY AGAIN -\n" RESET);
                    }
                }
            }
            else
            {
                printf(RED "\nInvalid option! Please enter 1 or 2.\n" RESET);
            }
        }
        else
        {
            printf(RED "----INVALID !,TRY AGAIN----\n\n" RESET);
        }
    }
    return 0;
}