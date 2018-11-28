#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define TRUE 1
#define FALSE 0
#define ON TRUE
#define OFF FALSE

#define startX 0
#define startY 0

#define C 'C'
#define H 'H'
#define N 'N'
#define D 'D'
#define P 'P'
#define P1 (P + 1)
#define P2 (P + 2)
#define P3 (P + 3)
#define P4 (P + 4)
#define P5 (P + 5)
#define G 'G'
#define T(c) ((c) >= '0' && (c) <= '9')

typedef struct queue_val {
	void *val;
	struct queue_val* next;
} QVAL;

typedef struct queue {
	QVAL *first;
	QVAL *last;
} QUEUE;

typedef struct point {
	int x;
	int y;
} POINT;

typedef struct path {
	int *path;
	int count;
	int time;
} PATH;

typedef struct part {
	PATH *path;
	struct part *next;
} PART;

typedef struct title {
	int count;
	int time;
	POINT *back;
} TITLE;

typedef struct list {
	int count;
	int time;
	PART *first;
} LIST;

typedef struct queue_item {
	int generator;
	int speed;	//0 standard, 1 slower 1/2
	POINT location;
	POINT *prev;
} QIT;

typedef struct teleport {
	POINT location;
	struct teleport *next;
} TELEPORT;

QUEUE *newQ() {
	return malloc(1 * sizeof(QUEUE));
}

void enQ(QUEUE *q, void *title) {
	if (q->first) {
		q->last->next = malloc(sizeof(QVAL));
		q->last = q->last->next;
	}
	else
		q->last = q->first = (QVAL *)malloc(sizeof(QVAL));

	q->last->val = title;
	q->last->next = NULL;
}

void pop(QUEUE *q) {
	if (q->first) {
		QVAL *tmp = q->first;
		q->first = q->first->next;
		free(tmp);
	}
}

void *top(QUEUE* q) {
	if (q->first)
		return q->first->val;

	return NULL;
}

int existsQ(QUEUE *q) {
	return q->first != NULL;
}

void clrQ(QUEUE* q) {
	while (q->first) {
		QVAL *tmp = q->first;
		q->first = q->first->next;
		free(tmp);
	}
}

void init(TITLE **dist, int n, int m, POINT p1, POINT p2, POINT p3, POINT p4, POINT gp) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++)
			dist[i][j].time = INT_MAX;
	}

	dist[p1.y][p1.x].count = INT_MAX;
	dist[p2.y][p2.x].count = INT_MAX;
	dist[p3.y][p3.x].count = INT_MAX;
	dist[p4.y][p4.x].count = INT_MAX;
	if (gp.x != -1)
		dist[gp.y][gp.x].count = INT_MAX;
}

QIT *newQIT(QIT *prev, int y, int x) {
	QIT *qit = malloc(sizeof(QIT));

	qit->speed = 0;	
	qit->location.x = x;
	qit->location.y = y;	
	qit->generator = prev->generator;
	qit->prev = (POINT *)&prev->location;	

	return qit;
}

void UDLR(int n, int m, QUEUE *q, QIT *val, POINT p) {
	if (p.y - 1 >= 0)
		enQ(q, newQIT(val, p.y - 1, p.x));
	if (p.x - 1 >= 0)
		enQ(q, newQIT(val, p.y, p.x - 1));
	if (p.y + 1 < n)
		enQ(q, newQIT(val, p.y + 1, p.x));
	if (p.x + 1 < m)
		enQ(q, newQIT(val, p.y, p.x + 1));
}

#define STEPS(location) dist[location.y][location.x].count
#define TIME(location) dist[location.y][location.x].time
#define MAP(location) mapa[location.y][location.x]
#define _STEPS(location) dist[location->y][location->x].count
#define _TIME(location) dist[location->y][location->x].time

void updateDist(TITLE** dist, QIT *val, int time) {
	TIME(val->location) = time;
	STEPS(val->location) = _STEPS(val->prev) + 1;
	dist[val->location.y][val->location.x].back = val->prev;
}

QIT *newStart(TITLE **dist, int x, int y, int gen) {
	QIT *val = malloc(sizeof(QIT));
	val->location.x = x;
	val->location.y = y;
	val->prev = &val->location;
	val->generator = gen;
	val->speed = 0;
	dist[y][x].time = 0;
	dist[y][x].count = 0;
	return val;
}

void addQ(char** mapa, int n, int m, TELEPORT **teleports, QUEUE *q, QIT *val) {
	if (val->generator && T(MAP(val->location))) {
		TELEPORT *tp = (TELEPORT*)teleports[MAP(val->location) - '0'];
		while (tp) {
			UDLR(n, m, q, val, tp->location);
			tp = tp->next;
		}
	}
	else
		UDLR(n, m, q, val, val->location);
}

void dijkstra(char** mapa, int n, int m, TELEPORT **teleports, QUEUE* q, TITLE **dist, int maxTime, POINT p1, POINT p2, POINT p3, POINT p4, POINT gp) {
	while (existsQ(q) && (STEPS(p1) == -1 || STEPS(p2) == -1 || STEPS(p3) == -1 || STEPS(p4) == -1 || (gp.x == -1 || STEPS(gp) == -1))) {
		QIT *val = top(q);
		pop(q);

		if (MAP(val->location) == N)
			continue;

		if (!val->speed++ && MAP(val->location) == H) {
			enQ(q, val);
			continue;
		}

		int t;
		if (MAP(val->location) == H)
			t = 2;
		else
			t = 1;

		t += _TIME(val->prev);

		if (t < 0 || t > maxTime)
			continue;

		if (t < TIME(val->location)) {
			updateDist(dist, val, t);
			addQ(mapa, n, m, teleports, q, val);
		}
	}

	clrQ(q);
}

void vytvorCestu(POINT ciel, TITLE **dist, PATH *pathBack) {
	if (STEPS(ciel) >= 0) {
		pathBack->count = STEPS(ciel);
		pathBack->time = TIME(ciel);
		int index = pathBack->count * 2;
		pathBack->path = malloc(index * sizeof(int));		
		POINT *p = (POINT*)&ciel;
		while (index > 0) {
			pathBack->path[--index] = p->x;
			pathBack->path[--index] = p->y;
			p = dist[p->y][p->x].back;
		}
	}
}

void vytvorStartDrak(int t, POINT Drak, POINT Generator, TITLE **dist, TITLE **distGen, PATH *startDrak, PATH *startGeneratorDrak) {
	if (Generator.x != -1 && distGen[Drak.y][Drak.x].count >= 0) {
		int index, offset = 0;
		if (STEPS(Generator) > 0) {
			startGeneratorDrak->count = distGen[Drak.y][Drak.x].count + STEPS(Generator);
			startGeneratorDrak->time = distGen[Drak.y][Drak.x].time + TIME(Generator);
			offset = STEPS(Generator) * 2;
		}
		else if (Generator.x != startX || Generator.y != startY)
			return;
		else {
			startGeneratorDrak->count = distGen[Drak.y][Drak.x].count;
			startGeneratorDrak->time = distGen[Drak.y][Drak.x].time;
		}

		// Ak sa k Drakovi pomocou generatora nedostanes ani v case t nema zmysel si to pamatat
		if (startGeneratorDrak->time <= t) {
			startGeneratorDrak->path = malloc(startGeneratorDrak->count * 2 * sizeof(int));
			index = distGen[Drak.y][Drak.x].count * 2;
			POINT *p = (POINT*)&Drak;
			while (index > 0) {
				startGeneratorDrak->path[offset + --index] = p->x;
				startGeneratorDrak->path[offset + --index] = p->y;
				p = distGen[p->y][p->x].back;
			}
			while (offset > 0) {
				startGeneratorDrak->path[--offset] = p->x;
				startGeneratorDrak->path[--offset] = p->y;
				p = dist[p->y][p->x].back;
			}
		}
		else printf("# Nedokazem vcas dobehnut k drakovi pomocou generatora\n");
	}
	else printf("# Nedokazem vcas dobehnut k drakovi pomocou generatora\n");

	// Ak cesta cez generator bola rychlejsie nema zmysel si pamatat cestu bez generatora
	if (STEPS(Drak) >= 0) {
		if (TIME(Drak) <= t) {
			if (startGeneratorDrak->path == NULL || STEPS(Drak) < startGeneratorDrak->count) {
				startDrak->count = dist[Drak.y][Drak.x].count;
				startDrak->time = dist[Drak.y][Drak.x].time;
				startDrak->path = malloc(startDrak->count * 2 * sizeof(int));
				int index = STEPS(Drak) * 2;
				POINT *p = (POINT*)&Drak;
				while (index > 0) {
					startDrak->path[--index] = p->x;
					startDrak->path[--index] = p->y;
					p = dist[p->y][p->x].back;
				}
			}
			else printf("# Pomocou generatora dokazem dobehnut k drakovi rychlejsie\n");
		}
		else printf("# Nedokazem vcas dobehnut k drakovi bez pomoci generatora\n");
	}
	else printf("# Nedokazem sa dostat k drakovi bez generatora\n");
}

void vypisCestu(PATH pathBack, char *cesta) {
	if (pathBack.path != NULL) {
		printf("%s v case %d po %d polickach\n", cesta, pathBack.time, pathBack.count);
		int i;
		for (i = 0; i < pathBack.count; ++i)
			printf("[%d;%d] ", pathBack.path[i * 2], pathBack.path[i * 2 + 1]);
		putchar('\n');
	}
}

void listUpdate(LIST* list, int n_args, ...) {
	int i, time = 0, count = 0;
	va_list ap;
	PATH *part;
	PART *partList = NULL, *tmp;

	va_start(ap, n_args);
	for (i = 1; i <= n_args; i++) {
		part = va_arg(ap, PATH*);

		if (part->path == NULL)	{
			va_end(ap);
			return;
		}

		tmp = malloc(sizeof(PART));
		tmp->next = partList;
		tmp->path = part;
		partList = tmp;

		time += part->time;
		count += part->count;

		if (time >= list->time) {
			va_end(ap);
			return;
		}
	}

	va_end(ap);

	if (list->first == NULL || time < list->time) {
		list->time = time;
		list->count = count;
		list->first = partList;
	}
}

void findLocations(char** mapa, int n, int m, POINT *Drogon, POINT *Princess1, POINT *Princess2, POINT *Princess3, POINT *Princess4, POINT *Princess5, POINT *Generator, TELEPORT **teleports) {
	int i, j, tmp = 1;
	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			if (mapa[i][j] == P) {
				mapa[i][j] += tmp++;
				switch (mapa[i][j]) {
					case P1:
						Princess1->y = i;
						Princess1->x = j;						
						break;
					case P2:
						Princess2->y = i;
						Princess2->x = j;						
						break;
					case P3:
						Princess3->y = i;
						Princess3->x = j;						
						break;
					case P4:
						Princess4->y = i;
						Princess4->x = j;
						break;
					case P5:
						Princess5->y = i;
						Princess5->x = j;
						break;
					default: break;
				}
			}
			else if (mapa[i][j] == D) {
				Drogon->y = i;
				Drogon->x = j;
			}
			else if (mapa[i][j] == G) {
				Generator->y = i;
				Generator->x = j;
			}
			else if (T(mapa[i][j])) {
				int index = mapa[i][j] - '0';
				TELEPORT *tp = malloc(sizeof(TELEPORT));
				tp->next = teleports[index];
				tp->location.y = i;
				tp->location.x = j;
				teleports[index] = tp;
			}
		}
	}
}

void fasterfrom2(POINT point1, POINT point2, PATH *path1, PATH *path2, TITLE **dist) {
	if (TIME(point1) == TIME(point2)) {
		vytvorCestu(point1, dist, path1);
		vytvorCestu(point2, dist, path2);
	}
	else if (TIME(point1) < TIME(point2))
		vytvorCestu(point1, dist, path1);
	else
		vytvorCestu(point2, dist, path2);
}

void fasterfrom3(POINT point1, POINT point2, POINT point3, PATH *path1, PATH *path2, PATH *path3, TITLE **dist) {
	if (TIME(point1) == TIME(point2)) {
		if (TIME(point1) == TIME(point3)) {
			// point1 == point2 == point3
			vytvorCestu(point1, dist, path1);
			vytvorCestu(point2, dist, path2);
			vytvorCestu(point3, dist, path3);
		}
		else if (TIME(point3) < TIME(point1)) {
			// point3 < point1 && point3 < point2
			vytvorCestu(point3, dist, path3);
		}
		else {
			// point1 < point3 && point2 < point3 && point1 == point2
			vytvorCestu(point1, dist, path1);
			vytvorCestu(point2, dist, path2);
		}
	}
	else if (TIME(point1) < TIME(point2))
		fasterfrom2(point1, point3, path1, path3, dist);
	else
		fasterfrom2(point2, point3, path2, path3, dist);
}

//START, DROGON, P1P2P3
void sdppp(PATH StartDrak, PATH DrakPrincenza1GV, PATH DrakPrincenza2GV, PATH DrakPrincenza3GV, PATH P1P2GN, PATH P1P3GN, PATH P2P1GN, PATH P2P3GN, PATH P3P1GN, PATH P3P2GN, LIST *list) {
	listUpdate(list, 4, &StartDrak, &DrakPrincenza1GV, &P1P2GN, &P2P3GN);
	listUpdate(list, 4, &StartDrak, &DrakPrincenza1GV, &P1P3GN, &P3P2GN);

	listUpdate(list, 4, &StartDrak, &DrakPrincenza2GV, &P2P1GN, &P1P3GN);
	listUpdate(list, 4, &StartDrak, &DrakPrincenza2GV, &P2P3GN, &P3P1GN);

	listUpdate(list, 4, &StartDrak, &DrakPrincenza3GV, &P3P1GN, &P1P2GN);
	listUpdate(list, 4, &StartDrak, &DrakPrincenza3GV, &P3P2GN, &P2P1GN);
}

//START, GENERATOR, DROGON, P1P2P3
void sgdppp(PATH StartGeneratorDrak, PATH DrakPrincenza1GZ, PATH DrakPrincenza2GZ, PATH DrakPrincenza3GZ, PATH P1P2GZ, PATH P1P3GZ, PATH P2P1GZ, PATH P2P3GZ, PATH P3P1GZ, PATH P3P2GZ, LIST *list) {
	listUpdate(list, 4, &StartGeneratorDrak, &DrakPrincenza1GZ, &P1P2GZ, &P2P3GZ);
	listUpdate(list, 4, &StartGeneratorDrak, &DrakPrincenza1GZ, &P1P3GZ, &P3P2GZ);

	listUpdate(list, 4, &StartGeneratorDrak, &DrakPrincenza2GZ, &P2P1GZ, &P1P3GZ);
	listUpdate(list, 4, &StartGeneratorDrak, &DrakPrincenza2GZ, &P2P3GZ, &P3P1GZ);

	listUpdate(list, 4, &StartGeneratorDrak, &DrakPrincenza3GZ, &P3P1GZ, &P1P2GZ);
	listUpdate(list, 4, &StartGeneratorDrak, &DrakPrincenza3GZ, &P3P2GZ, &P2P1GZ);
}

//START, DROGON, GENERATOR, P1P2P3
void sdgppp(PATH StartDrak, PATH DrakGenerator, PATH GeneratorPrincenza1, PATH GeneratorPrincenza2, 
	PATH GeneratorPrincenza3, PATH P1P2GZ, PATH P1P3GZ, PATH P2P1GZ, PATH P2P3GZ, PATH P3P1GZ, PATH P3P2GZ, LIST* list) {
	listUpdate(list, 5, &StartDrak, &DrakGenerator, &GeneratorPrincenza1, &P1P2GZ, &P2P3GZ);
	listUpdate(list, 5, &StartDrak, &DrakGenerator, &GeneratorPrincenza1, &P1P3GZ, &P3P1GZ);

	listUpdate(list, 5, &StartDrak, &DrakGenerator, &GeneratorPrincenza2, &P2P1GZ, &P1P3GZ);
	listUpdate(list, 5, &StartDrak, &DrakGenerator, &GeneratorPrincenza2, &P2P3GZ, &P3P1GZ);

	listUpdate(list, 5, &StartDrak, &DrakGenerator, &GeneratorPrincenza3, &P3P1GZ, &P1P2GZ);
	listUpdate(list, 5, &StartDrak, &DrakGenerator, &GeneratorPrincenza3, &P3P2GZ, &P2P1GZ);
}

//START, DROGON,  P1, GENERATOR, P2P3
void sdpgpp(PATH StartDrak, PATH DrakPrincenza1GV, PATH DrakPrincenza2GV, PATH DrakPrincenza3GV, 
	PATH GeneratorPrincenza1, PATH GeneratorPrincenza2, PATH GeneratorPrincenza3, PATH P1P2GZ, 
	PATH P1P3GZ, PATH P2P1GZ, PATH P2P3GZ, PATH P3P2GZ, PATH P1G, PATH P2G, PATH P3G, LIST* list) {
	listUpdate(list, 5, &StartDrak, &DrakPrincenza1GV, &P1G, &GeneratorPrincenza2, &P2P3GZ);
	listUpdate(list, 5, &StartDrak, &DrakPrincenza1GV, &P1G, &GeneratorPrincenza3, &P3P2GZ);

	listUpdate(list, 5, &StartDrak, &DrakPrincenza2GV, &P2G, &GeneratorPrincenza1, &P1P3GZ);
	listUpdate(list, 5, &StartDrak, &DrakPrincenza2GV, &P2G, &GeneratorPrincenza3, &P3P2GZ);

	listUpdate(list, 5, &StartDrak, &DrakPrincenza3GV, &P3G, &GeneratorPrincenza1, &P1P2GZ);
	listUpdate(list, 5, &StartDrak, &DrakPrincenza3GV, &P3G, &GeneratorPrincenza2, &P2P1GZ);
}

//START, DROGON,  P1, P2, GENERATOR, P3
void sdppgp(PATH StartDrak, PATH DrakPrincenza1GV, PATH DrakPrincenza2GV, PATH DrakPrincenza3GV, 
	PATH GeneratorPrincenza1, PATH GeneratorPrincenza2, PATH GeneratorPrincenza3, PATH P1P2GN, 
	PATH P1P3GN, PATH P2P1GN, PATH P2P3GN, PATH P3P1GN, PATH P3P2GN, PATH P1G, PATH P2G, PATH P3G, LIST *list) {
	listUpdate(list, 5, &StartDrak, &DrakPrincenza1GV, &P1P2GN, &P2G, &GeneratorPrincenza3);
	listUpdate(list, 5, &StartDrak, &DrakPrincenza1GV, &P1P3GN, &P3G, &GeneratorPrincenza2);

	listUpdate(list, 5, &StartDrak, &DrakPrincenza2GV, &P2P1GN, &P1G, &GeneratorPrincenza3);
	listUpdate(list, 5, &StartDrak, &DrakPrincenza2GV, &P2P3GN, &P3G, &GeneratorPrincenza1);

	listUpdate(list, 5, &StartDrak, &DrakPrincenza3GV, &P3P1GN, &P1G, &GeneratorPrincenza2);
	listUpdate(list, 5, &StartDrak, &DrakPrincenza3GV, &P3P2GN, &P2G, &GeneratorPrincenza1);
}

void startSearch(char** mapa, int n, int m, TELEPORT **teleports, POINT startPoint, POINT point1, 
	POINT point2, POINT point3, POINT point4, int gStatus, QUEUE* q, TITLE **dist) {
	init(dist, n, m, startPoint, point1, point2, point3, point4);
	QIT *start = newStart(dist, startPoint.x, startPoint.y, gStatus);
	UDLR(n, m, q, start, start->location);
	dijkstra(mapa, n, m, teleports, q, dist, INT_MAX, startPoint, point1, point2, point3, point4);
}

static void VypisCesty(PATH StartDrak, PATH StartGeneratorDrak, PATH DrakGenerator, PATH DrakPrincenza1GV, 
	PATH DrakPrincenza2GV, PATH DrakPrincenza3GV, PATH DrakPrincenza1GZ, PATH DrakPrincenza2GZ, PATH DrakPrincenza3GZ, 
	PATH GeneratorPrincenza1, PATH GeneratorPrincenza2, PATH GeneratorPrincenza3, PATH P1P2GZ, PATH P1P3GZ, PATH P2P1GZ, 
	PATH P2P3GZ, PATH P3P1GZ, PATH P3P2GZ, PATH P1P2GN, PATH P1P3GN, PATH P2P1GN, PATH P2P3GN, PATH P3P1GN, PATH P3P2GN, 
	PATH P1G, PATH P2G, PATH P3G) {
	vypisCestu(StartDrak, TOSTRING(StartDrak));
	vypisCestu(DrakGenerator, TOSTRING(DrakGenerator));
	vypisCestu(StartGeneratorDrak, TOSTRING(StartGeneratorDrak));

	vypisCestu(DrakPrincenza1GV, TOSTRING(DrakPrincenza1GV));
	vypisCestu(DrakPrincenza2GV, TOSTRING(DrakPrincenza2GV));
	vypisCestu(DrakPrincenza3GV, TOSTRING(DrakPrincenza3GV));

	vypisCestu(DrakPrincenza1GZ, TOSTRING(DrakPrincenza1GZ));
	vypisCestu(DrakPrincenza2GZ, TOSTRING(DrakPrincenza2GZ));
	vypisCestu(DrakPrincenza3GZ, TOSTRING(DrakPrincenza3GZ));

	vypisCestu(GeneratorPrincenza1, TOSTRING(GeneratorPrincenza1));
	vypisCestu(GeneratorPrincenza2, TOSTRING(GeneratorPrincenza2));
	vypisCestu(GeneratorPrincenza3, TOSTRING(GeneratorPrincenza3));

	vypisCestu(P1P2GZ, TOSTRING(P1P2GZ));
	vypisCestu(P1P3GZ, TOSTRING(P1P3GZ));
	vypisCestu(P2P1GZ, TOSTRING(P2P1GZ));
	vypisCestu(P2P3GZ, TOSTRING(P2P3GZ));
	vypisCestu(P3P1GZ, TOSTRING(P3P1GZ));
	vypisCestu(P3P2GZ, TOSTRING(P3P2GZ));

	vypisCestu(P1P2GN, TOSTRING(P1P2GN));
	vypisCestu(P1P3GN, TOSTRING(P1P3GN));
	vypisCestu(P2P1GN, TOSTRING(P2P1GN));
	vypisCestu(P2P3GN, TOSTRING(P2P3GN));
	vypisCestu(P3P1GN, TOSTRING(P3P1GN));
	vypisCestu(P3P2GN, TOSTRING(P3P2GN));

	vypisCestu(P1G, TOSTRING(P1G));
	vypisCestu(P2G, TOSTRING(P2G));
	vypisCestu(P3G, TOSTRING(P3G));
}

int* zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty) {
	int i;
	TELEPORT **teleports = malloc(10 * sizeof(TELEPORT*));
	POINT drogon, princess1, princess2, princess3, princess4, princess5, generator;
	generator.x = -1;

	// Zisti suradnice
	findLocations(mapa, n, m, &drogon, &princess1, &princess2, &princess3, &princess4, &princess5, &generator, teleports);

	QIT *start;
	QUEUE *q = newQ();
	PATH StartDrak, StartGeneratorDrak, DrakGenerator, DrakPrincenza1GV, DrakPrincenza2GV, DrakPrincenza3GV,
		DrakPrincenza1GZ, DrakPrincenza2GZ, DrakPrincenza3GZ, GeneratorPrincenza1, GeneratorPrincenza2, GeneratorPrincenza3,
		P1P2GZ, P1P3GZ, P2P1GZ, P2P3GZ, P3P1GZ, P3P2GZ, P1P2GN, P1P3GN, P2P1GN, P2P3GN, P3P1GN, P3P2GN, P1G, P2G, P3G;

	TITLE **dist = malloc(n * sizeof(TITLE*));
	TITLE **distGen = malloc(n * sizeof(TITLE*));
	for (i = 0; i < n; i++)	{
		dist[i] = malloc(m * sizeof(TITLE));
		distGen[i] = malloc(m * sizeof(TITLE));
	}

	P1G.path = P2G.path = P3G.path = DrakPrincenza1GV.path = DrakPrincenza2GV.path = DrakPrincenza3GV.path =
		DrakPrincenza1GZ.path = DrakPrincenza2GZ.path = DrakPrincenza3GZ.path =	GeneratorPrincenza1.path = 
		GeneratorPrincenza2.path = GeneratorPrincenza3.path = P1P2GZ.path = P1P3GZ.path = P2P1GZ.path = 
		P2P3GZ.path = P3P1GZ.path = P3P2GZ.path = P1P2GN.path = P1P3GN.path = P2P1GN.path = P2P3GN.path = 
		P3P1GN.path = P3P2GN.path = DrakGenerator.path = StartGeneratorDrak.path = StartDrak.path = NULL;

	init(dist, n, m, drogon, princess1, princess2, princess3, generator);
	start = newStart(dist, startX, startY, mapa[startY][startX] == G);
	UDLR(n, m, q, start, start->location);
	dijkstra(mapa, n, m, teleports, q, dist, t, drogon, princess1, princess2, princess3, generator);

	if (generator.x != -1) {
		init(distGen, n, m, drogon, princess1, princess2, princess3, generator);
		start = newStart(distGen, generator.x, generator.y, ON);
		UDLR(n, m, q, start, start->location);
		dijkstra(mapa, n, m, teleports, q, distGen, t, drogon, princess1, princess2, princess3, generator);
		fasterfrom3(princess1, princess2, princess3, &GeneratorPrincenza1, &GeneratorPrincenza2, &GeneratorPrincenza3, distGen);
	}
	vytvorStartDrak(t, drogon, generator, dist, distGen, &StartDrak, &StartGeneratorDrak);

	// PG
	if (generator.x != -1) {
		startSearch(mapa, n, m, teleports, princess1, drogon, princess2, princess3, generator, ON, q, dist);
		fasterfrom2(princess2, princess3, &P1P2GZ, &P1P3GZ, dist);

		startSearch(mapa, n, m, teleports, princess2, princess1, drogon, princess3, generator, ON, q, dist);
		fasterfrom2(princess1, princess3, &P2P1GZ, &P2P3GZ, dist);

		startSearch(mapa, n, m, teleports, princess3, princess1, princess2, drogon, generator, ON, q, dist);
		fasterfrom2(princess1, princess2, &P3P1GZ, &P3P2GZ, dist);

		startSearch(mapa, n, m, teleports, drogon, princess1, princess2, princess3, generator, ON, q, dist);
		fasterfrom3(princess1, princess2, princess3, &DrakPrincenza1GZ, &DrakPrincenza2GZ, &DrakPrincenza3GZ, dist);
	}

	// Drak->Princezne bez generatora
	if (StartDrak.path) {		
		startSearch(mapa, n, m, teleports, drogon, princess1, princess2, princess3, generator, OFF, q, dist);
		vytvorCestu(princess1, dist, &DrakPrincenza1GV);
		vytvorCestu(princess2, dist, &DrakPrincenza2GV);
		vytvorCestu(princess3, dist, &DrakPrincenza3GV);
		if (generator.x != -1)
			vytvorCestu(generator, dist, &DrakGenerator);

		startSearch(mapa, n, m, teleports, princess1, drogon, princess2, princess3, generator, OFF, q, dist);
		vytvorCestu(princess2, dist, &P1P2GN);
		vytvorCestu(princess3, dist, &P1P3GN);
		if (generator.x != -1)
			vytvorCestu(generator, dist, &P1G);

		startSearch(mapa, n, m, teleports, princess2, princess1, drogon, princess3, generator, OFF, q, dist);
		vytvorCestu(princess1, dist, &P2P1GN);
		vytvorCestu(princess3, dist, &P2P3GN);
		if (generator.x != -1)
			vytvorCestu(generator, dist, &P2G);

		startSearch(mapa, n, m, teleports, princess3, princess1, princess2, drogon, generator, OFF, q, dist);
		vytvorCestu(princess1, dist, &P3P1GN);
		vytvorCestu(princess2, dist, &P3P2GN);
		if (generator.x != -1)
			vytvorCestu(generator, dist, &P3G);
	}

	// Cesta k drakovi s generatorom
	if (StartGeneratorDrak.path) {
		// Existuje cesta Start->Drak->Generator v case t?
		if (DrakGenerator.path) {
			if (StartGeneratorDrak.time > DrakGenerator.time + StartDrak.time) {
				free(StartGeneratorDrak.path);
				StartGeneratorDrak.path = NULL;
				printf("# Aktivovat generator po drakovi je rychlejsie ako pred drakom\n");
			}
		}
	}

	LIST list;
	list.time = INT_MAX;
	list.first = NULL;

	// Start->Generator->Drak->Princezna->Princezna->Princezna
	if (StartGeneratorDrak.path)
		sgdppp(StartGeneratorDrak, DrakPrincenza1GZ, DrakPrincenza2GZ, DrakPrincenza3GZ, P1P2GZ, P1P3GZ, P2P1GZ, P2P3GZ, P3P1GZ, P3P2GZ, &list);

	// Start->Drak->...
	if (StartDrak.path) {
		// Start->Drak->Generator->Princezna->Princezna->Princezna
		if (DrakGenerator.path)
			sdgppp(StartDrak, DrakGenerator, GeneratorPrincenza1, GeneratorPrincenza2, GeneratorPrincenza3, P1P2GZ, P1P3GZ, P2P1GZ, P2P3GZ, P3P1GZ, P3P2GZ, &list);

		// Start->Drak->Princezna->Princezna->Princezna
		sdppp(StartDrak, DrakPrincenza1GV, DrakPrincenza2GV, DrakPrincenza3GV, P1P2GN, P1P3GN, P2P1GN, P2P3GN, P3P1GN, P3P2GN, &list);

		if (generator.x != -1) {
			// Start->Drak->Princezna->Generator->Princezna->Princezna
			sdpgpp(StartDrak, DrakPrincenza1GV, DrakPrincenza2GV, DrakPrincenza3GV, GeneratorPrincenza1, GeneratorPrincenza2, GeneratorPrincenza3, P1P2GZ, P1P3GZ, P2P1GZ, P2P3GZ, P3P2GZ, P1G, P2G, P3G, &list);

			// Start->Drak->Princezna->Princezna->Generator->Princezna
			sdppgp(StartDrak, DrakPrincenza1GV, DrakPrincenza2GV, DrakPrincenza3GV, GeneratorPrincenza1, GeneratorPrincenza2, GeneratorPrincenza3, P1P2GN, P1P3GN, P2P1GN, P2P3GN, P3P1GN, P3P2GN, P1G, P2G, P3G, &list);
		}
	}

	//VypisCesty(StartDrak, StartGeneratorDrak, DrakGenerator, DrakPrincenza1GV, DrakPrincenza2GV, DrakPrincenza3GV, DrakPrincenza1GZ, DrakPrincenza2GZ, DrakPrincenza3GZ, GeneratorPrincenza1, GeneratorPrincenza2, GeneratorPrincenza3, P1P2GZ, P1P3GZ, P2P1GZ, P2P3GZ, P3P1GZ, P3P2GZ, P1P2GN, P1P3GN, P2P1GN, P2P3GN, P3P1GN, P3P2GN, P1G, P2G, P3G);

	int* result;
	if (list.first)	{
		*dlzka_cesty = list.count;
		result = malloc(2 * list.count * sizeof(int));

		PART *part = list.first;
		int offset = 2 * list.count;

		while (part) {
			int end = part->path->count * 2;
			offset -= end;

			for (i = 0; i < end; i++)
				result[offset + i] = part->path->path[i];

			part = part->next;
		}
	}
	else {
		result = NULL;
		*dlzka_cesty = 0;
	}

	free(teleports);

	for (i = 0; i < n; i++) {
		free(dist[i]);
		free(distGen[i]);
	}

	free(dist);
	free(distGen);

	return result;
}

void main()
{
	const int n = 7;
	const int m = 7;
	const int t = 45;
	char** mapa = malloc(n * sizeof(char*));
	int i;
	for (i = 0; i < n; i++)
		mapa[i] = malloc(m * sizeof(char));
	i = 0;

	strncpy(mapa[i++], "HGHPCDC", m);
	strncpy(mapa[i++], "C0CCHPP", m);
	strncpy(mapa[i++], "HHHHHCH", m);
	strncpy(mapa[i++], "HHHHHHC", m);
	strncpy(mapa[i++], "0CCCHCH", m);
	strncpy(mapa[i++], "0CHCH1C", m);
	strncpy(mapa[i++], "CPCCCHH", m);



	int dlzka_cesty, j = 2;
	int* cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);

	printf("Zachranit vsetky princezne dokazem v %d krokoch\n", dlzka_cesty);
	for (i = 0; i < dlzka_cesty; ++i) {
		printf("cesta[%d] = %d;\n", j, cesta[i * 2 + 1]);
		j++;
		printf("cesta[%d] = %d;\n", j, cesta[i * 2]);
		j++;
	}
		
	/*
	for (i = 0; i < dlzka_cesty; ++i)
		printf("%d %d\n", cesta[i * 2], cesta[i * 2 + 1]);
	*/

	for (i = 0; i < n; i++)
		free(mapa[i]);
	free(mapa);

	getchar();
}