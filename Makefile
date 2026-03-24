# --- Variables ---
# Le compilateur à utiliser
CC = gcc

# Les options de compilation (-Wall et -Wextra affichent tous les avertissements utiles)
CFLAGS = -Wall -Wextra -g

# Le nom de ton programme final
EXEC = serveur

# La liste de tes fichiers sources
SRC = main.c jeu.c carte.c reseau.c mongoose.c

# La liste des fichiers objets (.o) générés automatiquement à partir de SRC
OBJ = $(SRC:.c=.o)


# --- Règles ---

# La règle par défaut quand on tape juste "make"
all: $(EXEC)

# Règle pour créer l'exécutable (relie tous les .o ensemble)
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# Règle générique pour transformer chaque .c en .o
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

# Règle pour nettoyer les fichiers temporaires (.o)
clean:
	rm -f *.o

# Règle pour tout nettoyer (les .o ET l'exécutable final)
mrproper: clean
	rm -f $(EXEC)