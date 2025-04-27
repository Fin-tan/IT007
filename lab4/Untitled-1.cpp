#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define N 1000
#define INF 1e9

struct process {
    char name[15];
    int id;
    float arr, brust, finish;
    float wait_time, rp_time, turnround_time;
    float remain_time;
};

int num_process;

// Queue Node
struct QNode {
    int id; // chỉ lưu id của process
    struct QNode* next;
};
typedef struct QNode* pQNode;

struct Queue {
    pQNode front, rear;
    int sz;
};
typedef struct Queue* pQueue;

pQNode CreateNode(int id) {
    pQNode temp = (pQNode)malloc(sizeof(struct QNode));
    temp->id = id;
    temp->next = NULL;
    return temp;
}

pQueue createQueue() {
    pQueue q = (pQueue)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    q->sz = 0;
    return q;
}

void push(pQueue q, int id) {
    pQNode temp = CreateNode(id);
    q->sz++;
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

int front(pQueue q) {
    if (q->front == NULL) return -1;
    return q->front->id;
}

void pop(pQueue q) {
    if (q->front == NULL) {
        q->sz = 0;
        return;
    }
    pQNode temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    q->sz--;
    free(temp);
}

int isEmpty(pQueue q) {
    return (q->sz == 0);
}

void freeQueue(pQueue q) {
    while (!isEmpty(q)) pop(q);
    free(q);
}

// Swap Process
void swap(struct process* a, struct process* b) {
    struct process tmp = *a;
    *a = *b;
    *b = tmp;
}

// Hiển thị
void show(struct process P[]) {
    for (int i = 0; i < num_process - 1; i++)
        for (int j = i + 1; j < num_process; j++)
            if (P[j].id < P[i].id) swap(&P[i], &P[j]);

    printf(" _______________________________________________________________________________________________________________ \n");
    printf("|  Process Name  | Arrival Time | Burst Time | Completion Time | Turnaround Time | Waiting Time | Response Time |\n");
    printf("|________________|______________|____________|_________________|_________________|______________|_______________|\n");

    float avg_waiting_time = 0;
    float avg_turnaround = 0;
    float avg_respone_time = 0;
    for (int i = 0; i < num_process; i++) {
        printf("|%-16s|%-14.2f|%-12.2f|%-17.2f|%-17.2f|%-14.2f|%-15.2f|\n",
               P[i].name, P[i].arr, P[i].brust, P[i].finish,
               P[i].turnround_time, P[i].wait_time, P[i].rp_time);
        avg_waiting_time += P[i].wait_time;
        avg_turnaround += P[i].turnround_time;
        avg_respone_time += P[i].rp_time;
    }

    printf("|________________|______________|____________|_________________|_________________|______________|_______________|\n");
    avg_waiting_time /= num_process;
    avg_turnaround /= num_process;
    avg_respone_time /= num_process;

    printf("Average waiting time : %.2f\n", avg_waiting_time);
    printf("Average turn around time: %.2f\n", avg_turnaround);
    printf("Average respone time: %.2f\n", avg_respone_time);
}

// Các hàm thuật toán (giống code gốc của bạn, chỉ sửa chỗ reset/khởi tạo nhỏ)

// Reset lại tiến trình
void reset(struct process P[]) {
    for (int i = 0; i < num_process; i++) {
        P[i].finish = P[i].turnround_time = P[i].wait_time = P[i].rp_time = 0;
        P[i].remain_time = P[i].brust;
    }
}

// Sort theo arrival time
void sort(struct process P[]) {
    for (int i = 0; i < num_process - 1; i++)
        for (int j = i + 1; j < num_process; j++)
            if (P[j].arr < P[i].arr || (P[j].arr == P[i].arr && P[j].id < P[i].id))
                swap(&P[i], &P[j]);
}

// FCFS
void FCFS(struct process P[]) {
    float time_elapsed = 0;
    for (int i = 0; i < num_process; i++) {
        if (time_elapsed < P[i].arr)
            time_elapsed = P[i].arr;
        P[i].wait_time = P[i].rp_time = time_elapsed - P[i].arr;
        time_elapsed += P[i].brust;
        P[i].finish = time_elapsed;
        P[i].turnround_time = P[i].finish - P[i].arr;
    }
    show(P);
}

// SJF (non-preemptive)
void SJF(struct process P[]) {
    int done[N] = {0};
    float time_elapsed = P[0].arr;
    for (int k = 0; k < num_process; k++) {
        int idx = -1;
        float min_burst = INF;
        for (int i = 0; i < num_process; i++) {
            if (!done[i] && P[i].arr <= time_elapsed && P[i].brust < min_burst) {
                min_burst = P[i].brust;
                idx = i;
            }
        }
        if (idx == -1) {
            for (int i = 0; i < num_process; i++) {
                if (!done[i]) {
                    idx = i;
                    time_elapsed = P[i].arr;
                    break;
                }
            }
        }
        P[idx].rp_time = time_elapsed - P[idx].arr;
        P[idx].wait_time = P[idx].rp_time;
        time_elapsed += P[idx].brust;
        P[idx].finish = time_elapsed;
        P[idx].turnround_time = P[idx].finish - P[idx].arr;
        done[idx] = 1;
    }
    show(P);
}

// SRTF (preemptive SJF)
int findSRTF(const struct process P[], int done[], float time_elapsed) {
    int idx = -1;
    float min_time = INF;
    for (int i = 0; i < num_process; i++) {
        if (!done[i] && P[i].arr <= time_elapsed && P[i].remain_time < min_time && P[i].remain_time > 0) {
            min_time = P[i].remain_time;
            idx = i;
        }
    }
    return idx;
}

void SRTF(struct process P[]) {
    float time_elapsed = P[0].arr;
    int done[N] = {0};
    int cnt = num_process;
    while (cnt > 0) {
        int idx = findSRTF(P, done, time_elapsed);
        if (idx == -1) {
            time_elapsed += 0.1;
            continue;
        }
        if (P[idx].remain_time == P[idx].brust)
            P[idx].rp_time = time_elapsed - P[idx].arr;

        P[idx].remain_time -= 0.1;
        time_elapsed += 0.1;

        if (P[idx].remain_time <= 0) {
            done[idx] = 1;
            cnt--;
            P[idx].finish = time_elapsed;
            P[idx].turnround_time = P[idx].finish - P[idx].arr;
            P[idx].wait_time = P[idx].turnround_time - P[idx].brust;
        }
    }
    show(P);
}

// RR
void RR(struct process P[]) {
    float quantum;
    printf("Enter Quantum time: ");
    scanf("%f", &quantum);

    pQueue q = createQueue();
    float time_elapsed = 0;
    int done[N] = {0};

    int numP = 0;
    int cntP = num_process;

    while (cntP > 0) {
        while (numP < num_process && P[numP].arr <= time_elapsed) {
            push(q, numP++);
        }
        if (isEmpty(q)) {
            if (numP < num_process)
                time_elapsed = P[numP].arr;
            continue;
        }
        int idx = front(q);
        pop(q);

        if (P[idx].remain_time == P[idx].brust)
            P[idx].rp_time = time_elapsed - P[idx].arr;

        float rtime = (P[idx].remain_time < quantum) ? P[idx].remain_time : quantum;
        time_elapsed += rtime;
        P[idx].remain_time -= rtime;

        while (numP < num_process && P[numP].arr <= time_elapsed) {
            push(q, numP++);
        }

        if (P[idx].remain_time == 0) {
            P[idx].finish = time_elapsed;
            P[idx].turnround_time = P[idx].finish - P[idx].arr;
            P[idx].wait_time = P[idx].turnround_time - P[idx].brust;
            done[idx] = 1;
            cntP--;
        } else {
            push(q, idx);
        }
    }
    freeQueue(q);
    show(P);
}

void Process_Scheduling_Algo() {
    struct process P[N];
    printf("Enter number of process: ");
    scanf("%d", &num_process);

    for (int i = 0; i < num_process; i++) {
        printf("Enter Process Name: ");
        scanf("%s", P[i].name);
        printf("Enter Arrival Time: ");
        scanf("%f", &P[i].arr);
        printf("Enter Burst Time: ");
        scanf("%f", &P[i].brust);
        P[i].id = i;
        P[i].remain_time = P[i].brust;
    }

    char algo[10];
    while (1) {
        printf("\nEnter algorithm (SJF, SRTF, RR, FCFS or EXIT): ");
        scanf("%s", algo);
        if (strcmp(algo, "EXIT") == 0) break;
        sort(P);
        reset(P);
        if (strcmp(algo, "SJF") == 0)
            SJF(P);
        else if (strcmp(algo, "SRTF") == 0)
            SRTF(P);
        else if (strcmp(algo, "RR") == 0)
            RR(P);
        else if (strcmp(algo, "FCFS") == 0)
            FCFS(P);
        else
            printf("Invalid Algorithm Name!\n");
    }
}

int main() {
    Process_Scheduling_Algo();
    return 0;
}
