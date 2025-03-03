all:$(SCR)
	$(CC) -o $(OBJECT) $^ $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

clean:
	rm -rf -o $(OBJECT)