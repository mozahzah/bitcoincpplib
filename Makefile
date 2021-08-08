LIBS=$(wildcard src/*)
INCLUDE_PATH=E:/Bitcoin/bitcoincpplib/include/
EXTERN_H=C:/msys64/mingw64/include/



lib:
	mkdir lib/
	for dir in $(LIBS); do \
		cd $$dir; \
    	g++ -I$(INCLUDE_PATH) -I$(EXTERN_H)boost/ -I$(EXTERN_H)openssl/ -I$(EXTERN_H)curl/ -LC:/msys64/mingw64/lib -lcurl -lssl -lcrypto -g -c *.cpp; \
    	mv *.o ../../lib; \
    	cd -; \
	done

clean:
	rm -rf lib/

