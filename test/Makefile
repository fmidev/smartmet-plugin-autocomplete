all: test

clean:
	rm -f $(PROG) *~

ifdef CI
test: fminames
else
test: 
endif
	@rm -f failures/*
	@echo Running tests:
	ok=true; smartmet-plugin-test --handler=/autocomplete --reactor-config=cnf/reactor.conf || ok=false; $$ok

fminames:
	sed -i -e 's/"smartmet-test"/"localhost"/g' cnf/*.conf
	@/usr/share/smartmet/test/db/init-and-start.sh && /usr/share/smartmet/test/db/install-test-db.sh > /dev/null
