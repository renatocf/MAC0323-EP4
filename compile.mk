ep4encenc: $(OBJ) word_enc.o lemma_enc.o
ep4enclp:  $(OBJ) word_enc.o lemma_lp.o
ep4lpenc:  $(OBJ) word_lp.o  lemma_enc.o
ep4lplp:   $(OBJ) word_lp.o  lemma_lp.o

%_lp.o: %.c
	$(CC) $(CLIBS) $(CFLAGS) -DLP=1 -c $< -o $@

%_enc.o: %.c
	$(CC) $(CLIBS) $(CFLAGS) -DENC=1 -c $< -o $@
