#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<curl/curl.h>

#define MAX_BUSES 10
#define MAX_SEATS 40
#define BUS_FILE "buses.txt"
#define BOOKING_FILE "bookings.txt"

#define FROM_EMAIL "busreservation457@gmail.com"
#define APP_PASSWORD "qhhqyzpyghsedhou"
#define SMTP_URL "smtps://smtp.gmail.com:465"

typedef struct 
{
    int id;
    char name[50];
    char source[30];
    char destination[30];
    int totalSeats;
    int seats[MAX_SEATS];

    char busType[30];
    char journeyDate[20];    
    char departureTime[20];    
    float fare;
} Bus;

typedef struct Booking 
{
    int busId;
    int seatNo;
    char pass_name[50];
    char email[50];
    char mobile_number[15];
    char source[30];
    char destination[30];
    int age;
    char gender[10];
    struct Booking *next;
} Booking;


Bus buses[MAX_BUSES];
int b_count = 0;
Booking *head = NULL;


void load_buses();
void save_bus_data();
void load_bookings_data();
void save_booking_info();
void add_new_bus_service();
void show_buses_list();
void display_seat_mapping(Bus *b);
void book_ticket();
void cancel_ticket();
void view_bookings_info();
void display_menu();
Bus* search_bus(int id);
void send_email_notification(const char *toEmail, const char *name, const char *source,const char *dest, int busId, int seatNo,const char *busType, const char *journeyDate,const char *departureTime, float fare,const char *mobile, int age, const char *gender);

struct upload_status 
{
    const char *readptr;
    size_t sizeleft;
};

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) 
{
    struct upload_status *upload_ctx=(struct upload_status *)userp;
    size_t buffer_size=size * nmemb;
    if(upload_ctx->sizeleft) 
    {
        size_t copy_size=(upload_ctx->sizeleft < buffer_size) ? upload_ctx->sizeleft : buffer_size;
        memcpy(ptr, upload_ctx->readptr, copy_size);
        upload_ctx->readptr += copy_size;
        upload_ctx->sizeleft -= copy_size;
        return copy_size;
    }
    return 0;
}


void send_email_notification(const char *toEmail, const char *name, const char *source,const char *dest, int busId, int seatNo,const char *busType, const char *journeyDate,const char *departureTime, float fare,const char *mobile, int age, const char *gender)
{
    CURL *curl;
    CURLcode res;
    char payload[8192];

    
    time_t now=time(NULL);
    struct tm *t=localtime(&now);
    char bookingDateTime[128];
    strftime(bookingDateTime, sizeof(bookingDateTime), "%Y-%m-%d %I:%M %p", t);

    snprintf(payload, sizeof(payload),
        "To: %s\r\n"
        "From: Bus Reservation System <%s>\r\n"
        "Subject: 🎟️ Ticket Confirmed - Bus %d\r\n"
        "\r\n"
        "🎟️ Ticket Confirmed - Bus %d\n\n"
        "🚌  BUS TICKET CONFIRMATION\n"
        "------------------------------------------\n"
        "Dear %s,\n\n"
        "🎉 Your bus ticket has been CONFIRMED successfully!\n\n"
        "📅  Journey Details:\n"
        "------------------------------------------\n"
        "🆔  Bus ID          : %d\n"
        "🚌  Bus Type        : %s\n"
        "🗓️  Journey Date    : %s\n"
        "⏰  Departure Time  : %s\n"
        "💺  Seat Number     : %d\n"
        "📍  From            : %s\n"
        "🎯  Destination     : %s\n"
        "💰  Fare            : ₹%.2f\n"
        "------------------------------------------\n\n"
        "👤 Passenger Info:\n"
        "------------------------------------------\n"
        "Name     : %s\n"
        "Age      : %d\n"
        "Gender   : %s\n"
        "Mobile   : %s\n"
        "Email    : %s\n"
        "------------------------------------------\n\n"
        "🕒 Booking Time: %s\n"
        "🕒 Please arrive at the boarding point 15 minutes before departure.\n"
        "🌟 Wishing you a safe and pleasant journey!\n\n"
        "— Bus Reservation System\n",
        toEmail, FROM_EMAIL, busId,busId, name, busId, busType, journeyDate, departureTime, seatNo, source, dest, fare,name, age, gender, mobile, toEmail,bookingDateTime
    );

    struct upload_status upload_ctx = { payload, strlen(payload) };
    struct curl_slist *recipients = NULL;

    curl=curl_easy_init();
    if(!curl) 
    {
	    fprintf(stderr, "❌ curl init failed\n");
	    return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, SMTP_URL);

    curl_easy_setopt(curl, CURLOPT_USERNAME, FROM_EMAIL);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, APP_PASSWORD);

    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);


    char from[100];
    sprintf(from, "<%s>", FROM_EMAIL);

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from);

    char to[100];
    sprintf(to, "<%s>", toEmail);

    recipients = curl_slist_append(recipients, to);

    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);

    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);

    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);


    res=curl_easy_perform(curl);
    if(res != CURLE_OK)
        fprintf(stderr, "❌ Email send failed: %s\n", curl_easy_strerror(res));
    else
        printf("📧 Confirmation email sent successfully to %s!\n", toEmail);

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
}



void save_bus_data() 
{
    FILE *fp=fopen(BUS_FILE, "w");
    if(!fp) 
    	return;
    for(int i=0;i<b_count;i++) 
    {
        fprintf(fp, "%d|%s|%s|%s|%d|%s|%s|%s|%.2f\n",buses[i].id, buses[i].name, buses[i].source, buses[i].destination,buses[i].totalSeats, buses[i].busType, buses[i].journeyDate,buses[i].departureTime, 				 buses[i].fare);
        for(int j=0;j<buses[i].totalSeats;j++)
            fprintf(fp, "%d ", buses[i].seats[j]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void load_buses() 
{
    FILE *fp=fopen(BUS_FILE, "r");
    if(!fp) 
    	return;
    char line[1024];
    b_count=0;
    while(fgets(line, sizeof(line), fp) && b_count < MAX_BUSES) 
    {
    
        Bus tmp;
        memset(&tmp, 0, sizeof(tmp));
        char *p=line;
            int n=sscanf(line, "%d|%49[^|]|%29[^|]|%29[^|]|%d|%29[^|]|%19[^|]|%19[^|]|%f",&tmp.id, tmp.name, tmp.source, tmp.destination, &tmp.totalSeats,tmp.busType, tmp.journeyDate, tmp.departureTime, &tmp.fare);
        if(n<9) 
        {
            fgets(line, sizeof(line), fp);
            continue;
        }
        
        if(!fgets(line, sizeof(line), fp)) 
        	break;
        for(int j=0;j < tmp.totalSeats && j < MAX_SEATS;j++)
         {
            int seatVal=0;
            if(sscanf(line + (j * 2), "%d", &seatVal)==1)
                tmp.seats[j]=seatVal;
            else
                tmp.seats[j]=0;
        }
        buses[b_count++]=tmp;
    }
    fclose(fp);
}


void save_booking_info()
{
    FILE *fp=fopen(BOOKING_FILE, "w");
    if(!fp) 
    	return;
    Booking *curr=head;
    while(curr) 
    {
        fprintf(fp, "%d|%d|%s|%s|%s|%s|%s|%d|%s\n",curr->busId, curr->seatNo, curr->pass_name, curr->email,curr->mobile_number, curr->source, curr->destination,curr->age, curr->gender);
        curr = curr->next;
    }
    fclose(fp);
}

void load_bookings_data() 
{
    FILE *fp=fopen(BOOKING_FILE, "r");
    if(!fp) 
    	return;
    char line[1024];
    while(fgets(line, sizeof(line), fp)) 
    {
        Booking temp;
        memset(&temp, 0, sizeof(temp));
            int n = sscanf(line, "%d|%d|%49[^|]|%49[^|]|%14[^|]|%29[^|]|%29[^|]|%d|%9[^|\n]",&temp.busId, &temp.seatNo, temp.pass_name, temp.email,temp.mobile_number, temp.source, temp.destination,
                       &temp.age, temp.gender);
        if(n==9) 
        {
            Booking *b=(Booking*)malloc(sizeof(Booking));
            if(!b) 
            	continue;
            *b=temp;
            b->next=head;
            head=b;
            Bus *bus=search_bus(b->busId);
            if(bus && b->seatNo >= 1 && b->seatNo <= bus->totalSeats)
                bus->seats[b->seatNo - 1] = 1;
        }
    }
    fclose(fp);
}



Bus* search_bus(int id) 
{
    for(int i=0;i< b_count; i++)
        if(buses[i].id == id) 
        	return &buses[i];
    return NULL;
}

void display_seat_mapping(Bus *b) 
{
    printf("\n\033[36m=========== Seat Layout for Bus ID %d (%s → %s) ===========\033[0m\n",
           b->id, b->source, b->destination);

    for (int i = 0; i < b->totalSeats; i++) {
        int seatNum = i + 1;
        int booked = b->seats[i];

        if (booked == 1) {
            // Find who booked this seat
            Booking *tmp = head;
            while (tmp) {
                if (tmp->busId == b->id && tmp->seatNo == seatNum) {
                    if (strcasecmp(tmp->gender, "Male") == 0)
                        printf("\033[32m[M]\033[0m ");   // Green for Male
                    else if (strcasecmp(tmp->gender, "Female") == 0)
                        printf("\033[35m[F]\033[0m ");   // Pink (Magenta) for Female
                    else
                        printf("\033[33m[X]\033[0m ");   // Yellow for others
                    break;
                }
                tmp = tmp->next;
            }
            if (!tmp) printf("\033[33m[X]\033[0m "); // fallback (shouldn't happen)
        } else {
            printf("\033[37m[%02d]\033[0m ", seatNum);  // White for available
        }

        // newline after every 4 seats for layout
        if (seatNum % 4 == 0)
            printf("\n");
    }

    printf("\n\033[32m[M]=Male\033[0m  \033[35m[F]=Female\033[0m  \033[37m[##]=Available\033[0m\n");
    printf("============================================================\n");
}


void show_buses_list() 
{
    printf("\n%-5s %-20s %-15s %-15s %-5s %-10s %-12s %-8s %-7s\n","ID", "Name", "Source", "Destination", "Seats", "Type", "Date", "DepTime", "Fare");
    for(int i=0;i<b_count; i++) 
    {
        printf("%-5d %-20s %-15s %-15s %-5d %-10s %-12s %-8s ₹%.2f\n",buses[i].id, buses[i].name, buses[i].source, buses[i].destination,buses[i].totalSeats, buses[i].busType, buses[i].journeyDate,
               buses[i].departureTime, buses[i].fare);
    }
}

void add_new_bus_service() 
{
    if(b_count >= MAX_BUSES) 
    {
        printf("Max bus limit reached.\n");
        return;
    }
    Bus b;
    memset(&b, 0, sizeof(b));
    b.id = b_count + 1;
    printf("\033[32mEnter Bus Name: \033[0m");
    scanf(" %49[^\n]", b.name);
    printf("\033[32mEnter Source: \033[0m");
    scanf(" %29[^\n]", b.source);
    printf("\033[32mEnter Destination: \033[0m");
    scanf(" %29[^\n]", b.destination);
    printf("\033[32mEnter Total Seats (<=%d): \033[0m", MAX_SEATS);
    scanf("%d", &b.totalSeats);
    if(b.totalSeats < 1 || b.totalSeats > MAX_SEATS) b.totalSeats = MAX_SEATS;
    for(int i=0;i<b.totalSeats;i++) 
    	b.seats[i] = 0;
    printf("\033[32mEnter Bus Type (e.g. AC/NON-AC): \033[0m");
    scanf(" %29[^\n]", b.busType);
    printf("\033[31mEnter Journey Date (YYYY-MM-DD): \033[0m");
    scanf(" %19[^\n]", b.journeyDate);
    printf("\033[32mEnter Departure Time (e.g. 09:30 PM): \033[0m");
    scanf(" %19[^\n]", b.departureTime);
    printf("\033[32Enter Fare(numeric): \033[0m");
    scanf("%f", &b.fare);

    buses[b_count++]=b;
    save_bus_data();
    printf("✅ Bus added and saved.\n");
}

void book_ticket() 
{
    int id, seat;
    show_buses_list();
    printf("Enter Bus ID: ");
    scanf("%d", &id);
    Bus *b = search_bus(id);
    if(!b) {
        printf("\033[31m❌ Invalid bus!\033[0m\n");
        return;
    }

    display_seat_mapping(b);
    printf("Enter seat number: ");
    scanf("%d", &seat);

    if(seat < 1 || seat > b->totalSeats || b->seats[seat - 1] == 1) {
        printf("\033[31m❌ Seat unavailable!\033[0m\n");
        return;
    }

    Booking *bk = malloc(sizeof(Booking));
    if(!bk) { 
        printf("❌ Memory allocation failed!\n"); 
        return; 
    }
    memset(bk, 0, sizeof(Booking));

    bk->busId = id;
    bk->seatNo = seat;

    printf("\033[32mEnter Passenger Name: \033[0m");
    scanf(" %49[^\n]", bk->pass_name);
    printf("\033[32mEnter Email: \033[0m");
    scanf(" %49s", bk->email);
    printf("\033[32mEnter Mobile number: \033[0m");
    scanf(" %14s", bk->mobile_number);
    printf("\033[32mEnter Age: \033[0m");
    scanf("%d", &bk->age);
    printf("\033[32mEnter Gender (Male/Female): \033[0m");
    scanf(" %9s", bk->gender);

      // --- New beside-seat restriction logic (mutual rule) ---
    int beside = -1;
    if (seat % 4 == 1) beside = seat + 1;
    else if (seat % 4 == 2) beside = seat - 1;
    else if (seat % 4 == 3) beside = seat + 1;
    else if (seat % 4 == 0) beside = seat - 1;

    if (beside >= 1 && beside <= b->totalSeats && b->seats[beside - 1] == 1) {
        // Check who is sitting beside
        Booking *tmp = head;
        while (tmp) {
            if (tmp->busId == id && tmp->seatNo == beside) {
                if (strcasecmp(bk->gender, tmp->gender) != 0) {
                    // Color for gender display
                    char genderColor[20];
                    if (strcasecmp(tmp->gender, "Male") == 0)
                        strcpy(genderColor, "\033[32mMale\033[0m");   // Green for Male
                    else
                        strcpy(genderColor, "\033[35mFemale\033[0m"); // Pink for Female

                    printf("\n\033[31m❌ Cannot book seat %d beside a %s passenger (seat %d).\033[0m\n",
                           seat, genderColor, beside);
                    free(bk);
                    return;
                }
                break;
            }
            tmp = tmp->next;
        }
    }


    strncpy(bk->source, b->source, sizeof(bk->source)-1);
    strncpy(bk->destination, b->destination, sizeof(bk->destination)-1);

    bk->next = head;
    head = bk;
    b->seats[seat - 1] = 1;

    save_booking_info();
    save_bus_data();

    printf("✅ Ticket booked successfully!\n");

    send_email_notification(bk->email, bk->pass_name, bk->source, bk->destination,
                            bk->busId, bk->seatNo, b->busType, b->journeyDate,
                            b->departureTime, b->fare, bk->mobile_number, bk->age, bk->gender);
}


void cancel_ticket() 
{
    printf("Cancellation feature not yet implemented.\n");
}

void view_bookings_info()
{
    Booking *c=head;
    printf("\n%-5s %-5s %-20s %-25s %-10s %-8s %-12s\n","Bus", "Seat", "Name", "Email", "Mobile", "Age", "Gender");
    while(c) 
    {
        printf("%-5d %-5d %-20s %-25s %-10s %-8d %-12s\n",c->busId, c->seatNo, c->pass_name, c->email, c->mobile_number, c->age, c->gender);
        c=c->next;
    }
}

void display_menu() 
{
    int ch;
    do{
        printf("\n1. View Buses\n2. Add Bus\n3. Book Ticket\n4. View Bookings\n5. Exit\nEnter choice: ");
        scanf("%d", &ch);
        switch(ch) 
        {
            case 1: 
            		show_buses_list(); 
            		break;
            case 2: 
            		add_new_bus_service(); 
            		break;
            case 3: 
            		book_ticket(); 
            		break;
            case 4: 
            		view_bookings_info(); 
            		break;
            case 5: 
            		printf("Exiting...\n"); 
            		break;
            default: 
            		printf("Invalid choice!\n");
        }
    } while (ch != 5);
}

int main() {
    load_buses();
    load_bookings_data();
    display_menu();
    return 0;
}

