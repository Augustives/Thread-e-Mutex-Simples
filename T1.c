#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define tamanho_array 50
#define n_threads_clientes 25
#define n_threads_voluntarios 25

pthread_mutex_t mutex_disponiveis;
pthread_mutex_t mutex_reparos;

struct Roupa {
	int codigo;
	char modelo[25];
	int preco;
	char tamanho[5];
};

char roupas[10][25] = {"camisa", "bermuda", "meia", "casaco", "calca", "cachecol", "tenis", "bone", "blusa", "vestido"};
char tamanho[5][5] = {"PP", "P", "M", "G", "GG"};
struct Roupa roupas_disponiveis[tamanho_array] = {0};
struct Roupa roupas_reparo[tamanho_array] = {0};

int codigo = 1;
int frequencia[tamanho_array];

void cliente_doa_roupa(char roupa[]) {
	struct Roupa str_roupa;
	str_roupa.codigo = codigo;
	codigo++;
	strcpy(str_roupa.modelo, roupa);
	int r = rand() % 200;
	str_roupa.preco = r;
	int r2 = rand() % 5;
	strcpy(str_roupa.tamanho, tamanho[r2]);
	int i;
	for (i=0; i<tamanho_array; i++) {
		if (roupas_reparo[i].codigo == -1) {
			roupas_reparo[i] = str_roupa;
			printf("Cliente doou roupa usada: %s - ID:%d\n", roupa, str_roupa.codigo);
			return;
		}
	}
}

void cliente_compra_roupa(char roupa[]) {
	int  i;
	for (i=0; i<tamanho_array; i++) {
	    if (roupas_disponiveis[i].codigo != -1) {
	        frequencia[i] = frequencia[i]+1;
	    }
		int value;
        value = strcmp(roupa , roupas_disponiveis[i].modelo);  
		if (value == 0) {
			printf("Cliente comprou roupa: %s - ID:%d\n", roupa, roupas_disponiveis[i].codigo);
			frequencia[i] = 0;
			struct Roupa str_roupa;
            str_roupa.codigo = -1;
			roupas_disponiveis[i] = str_roupa;
			return;
		}
	}
	printf("Cliente roupa nao encontrada em disponiveis: %s\n", roupa);	
}

void voluntario_move_roupa() {
    int i;
    for (i=0; i<tamanho_array; i++) {
        if (roupas_reparo[i].codigo != -1) {
        	struct Roupa str_roupa;
        	str_roupa.codigo = roupas_reparo[i].codigo;
        	strcpy(str_roupa.modelo, roupas_reparo[i].modelo);
        	str_roupa.preco = roupas_reparo[i].preco;
        	strcpy(str_roupa.tamanho, roupas_reparo[i].tamanho);
	        roupas_disponiveis[i] = str_roupa;

			struct Roupa str_roupa2;
            str_roupa2.codigo = -1;
			roupas_reparo[i] = str_roupa2;
			printf("Voluntario moveu para disponivel: %s - ID:%d\n", roupas_disponiveis[i].modelo, roupas_disponiveis[i].codigo);
			return;
		}
    }
}

void voluntario_doa_nova(char roupa[]) {
	struct Roupa str_roupa;
	str_roupa.codigo = codigo;
	codigo++;
	strcpy(str_roupa.modelo, roupa);
	int r = rand() % 200;
	str_roupa.preco = r;
	int r2 = rand() % 5;
	strcpy(str_roupa.tamanho, tamanho[r2]);
	int i;
	for (i=0; i<tamanho_array; i++) {
		if (roupas_disponiveis[i].codigo == -1) {
			roupas_disponiveis[i] = str_roupa;
			printf("Voluntario doou roupa nova: %s - ID:%d\n", roupa, str_roupa.codigo);
			return;
		}
	}    
}

void voluntario_remove_velha() {
    int i;
    int maior;
    int id;
    maior = -1;
    for (i=0; i<tamanho_array; i++) {
        if (frequencia[i] > maior && roupas_disponiveis[i].codigo != -1) {
            id = i;
            maior = frequencia[i];
        }
    }
    if (roupas_disponiveis[id].codigo != -1) {
        printf("Voluntario removeu roupa indesejada: %s - ID:%d\n", roupas_disponiveis[id].modelo, roupas_disponiveis[id].codigo);
        struct Roupa str_roupa;
        str_roupa.codigo = -1;
	    roupas_disponiveis[id] = str_roupa;
	    frequencia[id] = 0;
    }
}

void *t_function_cliente(void *arg) {

    int i = rand() % 10;
    pthread_mutex_lock(&mutex_disponiveis);
    cliente_compra_roupa(roupas[i]);
    pthread_mutex_unlock(&mutex_disponiveis);
    int tempo = rand() % 5000000;;
    usleep(tempo);
    int j = rand() % 2;
    if (j == 0) {
        int i = rand() % 10;
        pthread_mutex_lock(&mutex_reparos);
        cliente_doa_roupa(roupas[i]);
        pthread_mutex_unlock(&mutex_reparos);
    } else {
        int i = rand() % 10;
        pthread_mutex_lock(&mutex_disponiveis);
        cliente_compra_roupa(roupas[i]);
        pthread_mutex_unlock(&mutex_disponiveis);
    }
}

void *t_function_voluntario(void *arg) {

    int tempo = rand() % 5000000;
    usleep(tempo);
    int i = rand() % 3;
    if (i == 0) {
        pthread_mutex_lock(&mutex_disponiveis);
        pthread_mutex_lock(&mutex_reparos);
        voluntario_move_roupa();
        pthread_mutex_unlock(&mutex_disponiveis);
        pthread_mutex_unlock(&mutex_reparos);
    } else if (i == 1) {
        int i = rand() % 10;
        pthread_mutex_lock(&mutex_disponiveis);
        voluntario_doa_nova(roupas[i]);
        pthread_mutex_unlock(&mutex_disponiveis);
    } else {
        pthread_mutex_lock(&mutex_disponiveis);
        voluntario_remove_velha();
        pthread_mutex_unlock(&mutex_disponiveis);
    }
}

void test_case() {
    cliente_doa_roupa(roupas[0]);
    cliente_doa_roupa(roupas[1]);
    voluntario_doa_nova(roupas[2]);
    
    cliente_compra_roupa(roupas[0]);
    cliente_compra_roupa(roupas[2]);
    
    voluntario_move_roupa();
    cliente_compra_roupa(roupas[0]);

    voluntario_move_roupa();
    voluntario_remove_velha();
}


int main(int argc, char const *argv[]) {
    srand(time(NULL));
    
    int i;
    for (i=0; i<tamanho_array; i++) {
        struct Roupa str_roupa;
        str_roupa.codigo = -1;
        roupas_disponiveis[i] = str_roupa;
        roupas_reparo[i] = str_roupa;
        frequencia[i] = 0;
    }
    
    pthread_mutex_init(&mutex_disponiveis, NULL);
    pthread_mutex_init(&mutex_reparos, NULL);
    pthread_t threads_c[n_threads_clientes];
    pthread_t threads_v[n_threads_voluntarios];
    
    for (i=0; i<n_threads_clientes; i++) {
        pthread_create(&threads_c[i], NULL, t_function_cliente, NULL);
    }
    
    for (i=0; i<n_threads_voluntarios; i++) {
        pthread_create(&threads_v[i], NULL, t_function_voluntario, NULL);
    }
    
    for (i=0; i<n_threads_clientes; i++) {
        pthread_join(threads_c[i], NULL);
    }
    
    for (i=0; i<n_threads_voluntarios; i++) {
        pthread_join(threads_v[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex_disponiveis);
    pthread_mutex_destroy(&mutex_reparos);
    
	return 0;
}
