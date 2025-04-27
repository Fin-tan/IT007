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