#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define INF 999999999

struct graf_clienti{
	int nr_strazi;
	int **adiacenta;
};
typedef struct graf_clienti GRAFCL;

struct client{
	char *nume;
	int timp_metrou;
	int suma;
};
typedef struct client CLIENT;

struct statie{
	char *nume;
	int numar_clienti;
	CLIENT *client;
	GRAFCL graf;
};
typedef struct statie STATIE;

struct graf_metrou{
	int nr_rute;
	int nr_statii;
	int **adiacenta;
	STATIE *statie;
};
typedef struct graf_metrou GRAFM;

void Eroare(int err, char *functie, char *text)
{
	printf("******************\n");
	printf("[EROARE]: %s\n", text);
	printf("**In functia %s\n", functie);
	printf("**%s(nr. eroare: %d)\n", strerror(err), err);
	printf("******************\n");
}

void EliberareMemorieMatrice(int **a, int n)
{
	int i;
	for(i = 0; i < n; i++) free(a[i]);
	free(a);
}

void IndiceClient(CLIENT *client, int nr_clienti, char *nume, int *indice)
{
	int i;
	for(i = 0; i < nr_clienti; i++)
	{
		if(strcmp(nume, client[i].nume) == 0)
		{
			*indice = i;
			break;
		}
	}
}

void IndiceStatie(GRAFM *g, char *nume, int *indice)
{
	int i;
	for(i = 0; i < g->nr_statii; i++)
	{
		if(strcmp(nume, g->statie[i].nume) == 0)
		{
			*indice = i;
			break;
		}
	}
}

int CitireGraf(FILE *in, GRAFM **graf)
{
	GRAFM *g;
	int i, j, timp, distanta, indice1, indice2;
	char c;
	char nume[16], nume2[16];
	g = (GRAFM *) malloc(sizeof(GRAFM));
	if(g == NULL)
	{
		Eroare(errno, "CitireGraf", "Alocare dinamica esuata!");
		return 0;
	}
	fscanf(in, "%d", &g->nr_statii);
	g->statie = (STATIE *) malloc(g->nr_statii * sizeof(STATIE));
	if(g->statie == NULL)
	{
		Eroare(errno, "CitireGraf", "Alocare dinamica esuata!");
		return 0;
	}
	for(i = 0; i < g->nr_statii; i++)
	{
		fscanf(in, "%s", nume);
		g->statie[i].nume = (char *) malloc((strlen(nume) + 1) * sizeof(char));
		if(g->statie[i].nume == NULL)
		{
			Eroare(errno, "CitireGraf", "Alocare dinamica esuata!");
			return 0;
		}
		strcpy(g->statie[i].nume, nume);
		fscanf(in, "%d", &g->statie[i].numar_clienti);
		g->statie[i].client = (CLIENT *) malloc(g->statie[i].numar_clienti * sizeof(CLIENT));
		if(g->statie[i].client == NULL)
		{
			Eroare(errno, "CitireGraf", "Alocare dinamica esuata!");
			return 0;
		}
		for(j = 0; j < g->statie[i].numar_clienti; j++)
		{
			fscanf(in, "%c", &c);
			fscanf(in, "%s", nume);
			g->statie[i].client[j].nume = (char *) malloc((strlen(nume) + 1) * sizeof(char));
			if(g->statie[i].client[j].nume == NULL)
			{
				Eroare(errno, "CitireGraf", "Alocare dinamica esuata!");
				return 0;
			}
			strcpy(g->statie[i].client[j].nume, nume);
			fscanf(in, "%d", &g->statie[i].client[j].timp_metrou);
			fscanf(in, "%d", &g->statie[i].client[j].suma);
		}
		fscanf(in, "%d", &g->statie[i].graf.nr_strazi);
		if(g->statie[i].numar_clienti != 0)
		{
			g->statie[i].graf.adiacenta = (int **) malloc(g->statie[i].numar_clienti * sizeof(int *));
			if(g->statie[i].graf.adiacenta == NULL)
			{
				Eroare(errno, "CitireGraf", "Alocare dinamica esuata!");
				return 0;
			}
		}
		for(j = 0; j < g->statie[i].numar_clienti; j++)
		{
			g->statie[i].graf.adiacenta[j] = (int *) calloc(g->statie[i].numar_clienti, sizeof(int));
			if(g->statie[i].graf.adiacenta[j] == NULL)
			{
				Eroare(errno, "CitireGraf", "Alocare dinamica esuata!");
				return 0;
			}
		}
		for(j = 0; j < g->statie[i].graf.nr_strazi; j++)
		{
			fscanf(in, "%s", nume);
			fscanf(in, "%c", &c);
			fscanf(in, "%s", nume2);
			fscanf(in, "%d", &timp);
			IndiceClient(g->statie[i].client, g->statie[i].numar_clienti, nume, &indice1);
			IndiceClient(g->statie[i].client, g->statie[i].numar_clienti, nume2, &indice2);
			g->statie[i].graf.adiacenta[indice1][indice2] = timp;
			g->statie[i].graf.adiacenta[indice2][indice1] = timp;
		}
	}
	fscanf(in, "%d", &g->nr_rute);
	if(g->nr_statii != 0)
	{
		g->adiacenta = (int **) malloc(g->nr_statii * sizeof(int *));
		if(g->adiacenta == NULL)
		{
			Eroare(errno, "CitireGraf", "Alocare dinamica esuata!");
			return 0;
		}
	}
	for(i = 0; i < g->nr_statii; i++)
	{
		g->adiacenta[i] = (int *) calloc(g->nr_statii, sizeof(int));
		if(g->adiacenta[i] == NULL)
		{
			Eroare(errno, "CitireGraf", "Alocare dinamica esuata!");
			return 0;
		}
	}
	for(i = 0; i < g->nr_rute; i++)
	{
		fscanf(in, "%s", nume);
		fscanf(in, "%c", &c);
		fscanf(in, "%s", nume2);
		fscanf(in, "%d", &distanta);
		IndiceStatie(g, nume, &indice1);
		IndiceStatie(g, nume2, &indice2);
		g->adiacenta[indice1][indice2] = distanta;
		g->adiacenta[indice2][indice1] = distanta;
	}
	*graf = g;
	return 1;
}

void Legatura(GRAFM *g, char *statie, FILE *out)
{
	int indice, i;
	IndiceStatie(g, statie, &indice);
	for(i = 0; i < g->nr_statii; i++)
	{
		if(g->adiacenta[indice][i] != 0 && g->adiacenta[indice][i] != INF)
		{
			fprintf(out, "%s ", g->statie[i].nume);
		}
	}
	fprintf(out, "\n");
}

void AdaugaRuta(GRAFM *g, char *statie1, char *statie2, int distanta)
{
	int indice1, indice2;
	IndiceStatie(g, statie1, &indice1);
	IndiceStatie(g, statie2, &indice2);
	g->adiacenta[indice1][indice2] = distanta;
	g->adiacenta[indice2][indice1] = distanta;
}

void StergeRuta(GRAFM *g, char *statie1, char *statie2)
{
	int indice1, indice2;
	IndiceStatie(g, statie1, &indice1);
	IndiceStatie(g, statie2, &indice2);
	g->adiacenta[indice1][indice2] = 0;
	g->adiacenta[indice2][indice1] = 0;
}

void ComandaStatie(FILE *out, GRAFM *g, int suma)
{
	int i, j, s;
	for(i = 0; i < g->nr_statii; i++)
	{
		s = 0;
		for(j = 0; j < g->statie[i].numar_clienti; j++) s += g->statie[i].client[j].suma;
		if(suma <= s) fprintf(out, "%s ", g->statie[i].nume);
	}
	fprintf(out, "\n");
}
int TimpStatie(FILE *out, GRAFM *g, char *statie)
{
	int indice, i, j, min, imin, nod_plecare, *vizitat, s = 0;
	IndiceStatie(g, statie, &indice);
	vizitat = (int *) calloc(g->statie[indice].numar_clienti, sizeof(int));
	if(vizitat == NULL)
	{
		Eroare(errno, "TimpStatie", "Alocare dinamica esuata!");
		return 0;
	}
	min = g->statie[indice].client[0].timp_metrou;
	nod_plecare = 0;
	for(i = 0; i < g->statie[indice].numar_clienti; i++)
	{
		if(min > g->statie[indice].client[i].timp_metrou)
		{
			nod_plecare = i;
			min = g->statie[indice].client[i].timp_metrou;
		}
	}
	s += min;
	i = 1;
	vizitat[nod_plecare] = 1;
	while(i < g->statie[indice].numar_clienti)
	{
		min = INF;
		for(j = 0; j < g->statie[indice].numar_clienti; j++)
		{
			if(g->statie[indice].graf.adiacenta[nod_plecare][j] != 0 && vizitat[j] == 0)
			{
				if(min == INF || g->statie[indice].graf.adiacenta[nod_plecare][j] < min)
				{
					min = g->statie[indice].graf.adiacenta[nod_plecare][j];
					imin = j;
				}
			}
		}
		s += min;
		nod_plecare = imin;
		vizitat[imin] = 1;
		i++;
	}
	s += g->statie[indice].client[nod_plecare].timp_metrou;
	fprintf(out, "%d\n", s);
	free(vizitat);
	return 1;
}

void GasesteClient(GRAFM *g, char *nume, int *indice_client, int *indice_statie)
{
	int i, j;
	for(i = 0; i < g->nr_statii; i++)
	{
		for(j = 0; j < g->statie[i].numar_clienti; j++)
		{
			if(strcmp(g->statie[i].client[j].nume, nume) == 0)
			{
				*indice_client = j;
				*indice_statie = i;
				return;
			}
		}
	}
}

void AdaugaStrada(GRAFM *g, char *client1, char *client2, int distanta)
{
	int indice_statie, indice_client1, indice_client2;
	GasesteClient(g, client1, &indice_client1, &indice_statie);
	IndiceClient(g->statie[indice_statie].client, g->statie[indice_statie].numar_clienti, client2, &indice_client2);
	g->statie[indice_statie].graf.adiacenta[indice_client1][indice_client2] = distanta;
	g->statie[indice_statie].graf.adiacenta[indice_client2][indice_client1] = distanta;
}

void StergeStrada(GRAFM *g, char *client1, char *client2)
{
	int indice_statie, indice_client1, indice_client2;
	GasesteClient(g, client1, &indice_client1, &indice_statie);
	IndiceClient(g->statie[indice_statie].client, g->statie[indice_statie].numar_clienti, client2, &indice_client2);
	g->statie[indice_statie].graf.adiacenta[indice_client1][indice_client2] = 0;
	g->statie[indice_statie].graf.adiacenta[indice_client2][indice_client1] = 0;
}

int AfisareRutaMetrou(FILE *out, GRAFM *g, int sursa, int destinatie, int *prec)
{
	int *v, i, k = 0;
	v = (int *) malloc(g->nr_statii * sizeof(int));
	if(v == NULL)
	{
		Eroare(errno, "AfisareRutaMetrou", "Alocare dinamica esuata!");
		return 0;
	}
	i = destinatie;
	while(prec[i] != -1)
	{
		v[k] = i;
		k++;
		i = prec[i];
	}
	v[k] = i;
	k++;
	for(i = k - 1; i >= 0; i--)
	{
		fprintf(out, "%s ", g->statie[v[i]].nume);
	}
	fprintf(out, "\n");
	free(v);
	return 1;
}

int DijkstraMetrou(FILE *out, GRAFM *g, int sursa, int destinatie)
{
	int i, j, min, dmin, *d = NULL, *prec = NULL, *cale = NULL;
	d = (int *) malloc(g->nr_statii * sizeof(int));
	if(d == NULL)
	{
		Eroare(errno, "DijkstraMetrou", "Alocare dinamica esuata!");
		return 0;
	}
	prec = (int *) malloc(g->nr_statii * sizeof(int));
	if(prec == NULL)
	{
		Eroare(errno, "DijkstraMetrou", "Alocare dinamica esuata!");
		return 0;
	}
	cale = (int *) malloc(g->nr_statii * sizeof(int));
	if(cale == NULL)
	{
		Eroare(errno, "DijkstraMetrou", "Alocare dinamica esuata!");
		return 0;
	}
	for(i = 0; i < g->nr_statii; i++)
	{
		d[i] = INF;
		cale[i] = 0;
	}
	d[sursa] = 0;
	prec[sursa] = -1;
	for(i = 0; i < g->nr_statii; i++)
	{
		dmin = INF;
		for(j = 0; j < g->nr_statii; j++)
		{
			if(cale[j] == 0 && d[j] <= dmin)
			{
				dmin = d[j];
				min = j;
			}
		}
		cale[min] = 1;
		for(j = 0; j < g->nr_statii; j++)
		{
			if(cale[j]==0 && g->adiacenta[min][j]!=0 && g->adiacenta[min][j]!=INF && d[min] != INF && d[min]+g->adiacenta[min][j] < d[j])
			{
				d[j] = d[min] + g->adiacenta[min][j];
				prec[j] = min;
			}
		}
	}
	if(AfisareRutaMetrou(out, g, sursa, destinatie, prec) == 0) return 0;
	free(d);
	free(cale);
	free(prec);
	return 1;
}

int DrumMetrou(FILE *out, GRAFM *g, char *statie1, char *statie2)
{
	int indice1, indice2;
	IndiceStatie(g, statie1, &indice1);
	IndiceStatie(g, statie2, &indice2);
	if(DijkstraMetrou(out, g, indice1, indice2) == 0) return 0;
	return 1;
}

int AfisareRutaStatie(FILE *out, GRAFM *g, int sursa, int destinatie, int statie, int *prec)
{
	int *v, i, k = 0;
	v = (int *) malloc(g->statie[statie].numar_clienti * sizeof(int));
	if(v == NULL)
	{
		Eroare(errno, "AfisareRutaStatie", "Alocare dinamica esuata!");
		return 0;
	}
	i = destinatie;
	while(prec[i] != -1)
	{
		v[k] = i;
		k++;
		i = prec[i];
	}
	v[k] = i;
	k++;
	for(i = k - 1; i >= 0; i--)
	{
		fprintf(out, "%s ", g->statie[statie].client[v[i]].nume);
	}
	fprintf(out, "\n");
	free(v);
	return 1;
}

int DijkstraStatie(FILE *out, GRAFM *g, int sursa, int destinatie, int statie)
{
	int i, j, min, dmin, *d = NULL, *prec = NULL, *cale = NULL;
	d = (int *) malloc(g->statie[statie].numar_clienti * sizeof(int));
	if(d == NULL)
	{
		Eroare(errno, "DijkstraStatie", "Alocare dinamica esuata!");
		return 0;
	}
	prec = (int *) malloc(g->statie[statie].numar_clienti * sizeof(int));
	if(prec == NULL)
	{
		Eroare(errno, "DijkstraStatie", "Alocare dinamica esuata!");
		return 0;
	}
	cale = (int *) malloc(g->statie[statie].numar_clienti * sizeof(int));
	if(cale == NULL)
	{
		Eroare(errno, "DijkstraStatie", "Alocare dinamica esuata!");
		return 0;
	}
	for(i = 0; i < g->statie[statie].numar_clienti; i++)
	{
		d[i] = INF;
		cale[i] = 0;
	}
	d[sursa] = 0;
	prec[sursa] = -1;
	for(i = 0; i < g->statie[statie].numar_clienti; i++)
	{
		dmin = INF;
		for(j = 0; j < g->statie[statie].numar_clienti; j++)
		{
			if(cale[j] == 0 && d[j] <= dmin)
			{
				dmin = d[j];
				min = j;
			}
		}
		cale[min] = 1;
		for(j = 0; j < g->statie[statie].numar_clienti; j++)
		{
			if(cale[j]==0 && g->statie[statie].graf.adiacenta[min][j]!=0 && g->statie[statie].graf.adiacenta[min][j]!=INF && 
			d[min] != INF && d[min]+g->statie[statie].graf.adiacenta[min][j] < d[j])
			{
				d[j] = d[min] + g->statie[statie].graf.adiacenta[min][j];
				prec[j] = min;
			}
		}
	}
	if(AfisareRutaStatie(out, g, sursa, destinatie, statie, prec) == 0) return 0;
	free(d);
	free(cale);
	free(prec);
	return 1;
}

int DrumStrada(FILE *out, GRAFM *g, char *client1, char *client2)
{
	int indice1, indice2, indice_statie;
	GasesteClient(g, client1, &indice1, &indice_statie);
	IndiceClient(g->statie[indice_statie].client, g->statie[indice_statie].numar_clienti, client2, &indice2);
	if(DijkstraStatie(out, g, indice1, indice2, indice_statie) == 0) return 0;
	return 1;
}

void Conexiune(FILE *out, GRAFM *g, char *client1, char *client2)
{
	int indice_statie, indice_client1, indice_client2;
	GasesteClient(g, client1, &indice_client1, &indice_statie);
	IndiceClient(g->statie[indice_statie].client, g->statie[indice_statie].numar_clienti, client2, &indice_client2);
	if(g->statie[indice_statie].graf.adiacenta[indice_client1][indice_client2] != 0) fprintf(out, "OK\n");
	else fprintf(out, "NO\n");
}

void BlocheazaTunel(GRAFM *g, char *statie1, char *statie2)
{
	int indice1, indice2;
	IndiceStatie(g, statie1, &indice1);
	IndiceStatie(g, statie2, &indice2);
	if(g->adiacenta[indice1][indice2] != 0)
	{
		g->adiacenta[indice1][indice2] = INF;
		g->adiacenta[indice2][indice1] = INF;
	}
}

void BlocheazaStrada(GRAFM *g, char *client1, char *client2)
{
	int indice_statie, indice_client1, indice_client2;
	GasesteClient(g, client1, &indice_client1, &indice_statie);
	IndiceClient(g->statie[indice_statie].client, g->statie[indice_statie].numar_clienti, client2, &indice_client2);
	if(g->statie[indice_statie].graf.adiacenta[indice_client1][indice_client2] != 0)
	{
		g->statie[indice_statie].graf.adiacenta[indice_client1][indice_client2] = INF;
		g->statie[indice_statie].graf.adiacenta[indice_client2][indice_client1] = INF;
	}
}

int CitireCerinte(FILE *in, FILE *out, GRAFM *g)
{
	int nr_operatii, i;
	char operatie[32];
	fscanf(in, "%d", &nr_operatii);
	for(i = 0; i < nr_operatii; i++)
	{
		fscanf(in, "%s", operatie);
		if(strcmp(operatie, "conexiune") == 0)
		{
			char c;
			char client1[16], client2[16];
			fscanf(in, "%c", &c);
			fscanf(in, "%s", client1);
			fscanf(in, "%c", &c);
			fscanf(in, "%s", client2);
			Conexiune(out, g, client1, client2);
		}
		else if(strcmp(operatie, "legatura") == 0)
		{
			char c;
			char statie[16]; 
			fscanf(in, "%c", &c);
			fscanf(in, "%s", statie);
			Legatura(g, statie, out);
		}
		else if(strcmp(operatie, "blocaj_tunel") == 0)
		{
			char c;
			char statie1[16], statie2[16];
			fscanf(in, "%c", &c);
			fscanf(in, "%s", statie1);
			fscanf(in, "%c", &c);
			fscanf(in, "%s", statie2);
			BlocheazaTunel(g, statie1, statie2);
		}
		else if(strcmp(operatie, "blocaj_strada") == 0)
		{
			char c;
			char client1[16], client2[16];
			fscanf(in, "%c", &c);
			fscanf(in, "%s", client1);
			fscanf(in, "%c", &c);
			fscanf(in, "%s", client2);
			BlocheazaStrada(g, client1, client2);
		}
		else if(strcmp(operatie, "adauga_ruta") == 0)
		{
			char c;
			int distanta;
			char statie1[16], statie2[16];
			fscanf(in, "%c", &c);
			fscanf(in, "%s", statie1);
			fscanf(in, "%c", &c);
			fscanf(in, "%s", statie2);
			fscanf(in, "%d", &distanta);
			AdaugaRuta(g, statie1, statie2, distanta);
		}
		else if(strcmp(operatie, "sterge_ruta") == 0)
		{
			char c;
			char statie1[16], statie2[16];
			fscanf(in, "%c", &c);
			fscanf(in, "%s", statie1);
			fscanf(in, "%c", &c);
			fscanf(in, "%s", statie2);
			StergeRuta(g, statie1, statie2);
		}
		else if(strcmp(operatie, "adauga_strada") == 0)
		{
			char c;
			int distanta;
			char client1[16], client2[16];
			fscanf(in, "%c", &c);
			fscanf(in, "%s", client1);
			fscanf(in, "%c", &c);
			fscanf(in, "%s", client2);
			fscanf(in, "%d", &distanta);
			AdaugaStrada(g, client1, client2, distanta);
		}
		else if(strcmp(operatie, "sterge_strada") == 0)
		{
			char c;
			char client1[16], client2[16];
			fscanf(in, "%c", &c);
			fscanf(in, "%s", client1);
			fscanf(in, "%c", &c);
			fscanf(in, "%s", client2);
			StergeStrada(g, client1, client2);
		}
		else if(strcmp(operatie, "drum_strada") == 0)
		{
			char c;
			char client1[16], client2[16];
			fscanf(in, "%c", &c);
			fscanf(in, "%s", client1);
			fscanf(in, "%c", &c);
			fscanf(in, "%s", client2);
			if(DrumStrada(out, g, client1, client2) == 0) return 0;
		}
		else if(strcmp(operatie, "drum_metrou") == 0)
		{
			char c;
			char statie1[16], statie2[16];
			fscanf(in, "%c", &c);
			fscanf(in, "%s", statie1);
			fscanf(in, "%c", &c);
			fscanf(in, "%s", statie2);
			if(DrumMetrou(out, g, statie1, statie2) == 0) return 0;
		}
		else if(strcmp(operatie, "timp_statie") == 0)
		{
			char c;
			char statie[16]; 
			fscanf(in, "%c", &c);
			fscanf(in, "%s", statie);
			if(TimpStatie(out, g, statie) == 0) return 0;
		}
		else if(strcmp(operatie, "comanda_statie") == 0)
		{
			int suma;
			fscanf(in, "%d", &suma);
			ComandaStatie(out, g, suma);
		}
	}
	return 1;
}

int CitireMatrice(FILE *in, FILE *out, GRAFM *g)
{
	int n;
	char str[16] = "0";
	fgets(str, 16, in);
	fgets(str, 16, in);
	n = atoi(str);
	if(n != 0)
	{
		int **a = NULL, i, j;
		a = (int **) malloc(n * sizeof(int *));
		if(a == NULL)
		{
			Eroare(errno, "CitireMatrice", "Alocare dinamica esuata!");
			return 0;
		}
		for(i = 0; i < n; i++)
		{
			a[i] = (int *) malloc(n * sizeof(int));
			if(a[i] == NULL)
			{
				Eroare(errno, "CitireMatrice", "Alocare dinamica esuata!");
				return 0;
			}
		}
		for(i = 0; i < n; i++)
			for(j = 0; j < n; j++) fscanf(in, "%d", &a[i][j]);

		for(i = 1; i < n; i++)
		{
			a[0][i] = a[0][i-1] + a[0][i];
			a[i][0] = a[i-1][0] + a[i][0];
		}
		for(i = 1; i < n; i++)
		{
			for(j = 1; j < n; j++)
			{
				if(a[i-1][j] < a[i][j-1]) a[i][j] = a[i-1][j] + a[i][j];
				else a[i][j] = a[i][j-1] + a[i][j];
			}
		}
		fprintf(out, "%d", a[n-1][n-1]);
		EliberareMemorieMatrice(a, n);
	}
	return 1;
}

int DeschideFisiere(FILE **in, FILE **out, char *argv[])
{
	if((*in = fopen(argv[1], "r")) == NULL)
	{
		Eroare(errno, "DeschideFisiere", "Deschiderea fisierului input a esuat!");
		return 0;
	}
	if((*out = fopen(argv[2], "w")) == NULL)
	{
		Eroare(errno, "DeschideFisiere", "Deschiderea fisierului output a esuat!");
		return 0;
	}
	return 1;
}

void EliberareMemorie(GRAFM *g)
{
	int i, j;
	for(i = 0; i < g->nr_statii; i++)
	{
		free(g->adiacenta[i]);
		for(j = 0; j < g->statie[i].numar_clienti; j++)
		{
			free(g->statie[i].client[j].nume);
			free(g->statie[i].graf.adiacenta[j]);
		}
		free(g->statie[i].graf.adiacenta);
		free(g->statie[i].client);
		free(g->statie[i].nume);
	}
	free(g->statie);
	free(g->adiacenta);
	free(g);
}

void InchideFisiere(FILE *in, FILE *out)
{
	fclose(in);
	fclose(out);
}

int main(int argc, char *argv[])
{
	GRAFM *graf;
	FILE *in, *out;
	if(DeschideFisiere(&in, &out, argv) == 0) return 0;
	if(CitireGraf(in, &graf) == 0) return 0;
	if(CitireCerinte(in, out, graf) == 0) return 0;
	if(CitireMatrice(in, out, graf) == 0) return 0;
	EliberareMemorie(graf);
	InchideFisiere(in, out);
	return 0;
}