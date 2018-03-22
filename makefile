#// Copyright (c) 2018 BUPT. All rights reserved.
#// pangwei pangweitf@163.com
#// Beijing Key Lab of Intelligent Telecomm. Software and Multimedia,Beijing Univ. of Posts and         Telecomm., Beijing 100876, China


CC = g++

INC= -I./ -I./thirdparty/ann/include \
	 -I./thirdparty/btrie/include
LIBS=-L./thirdparty/ann/lib -lANN \
	 -L./thirdparty/btrie/lib -ldtrie

CPPFLAGS_CN= -O3 -Wall -Wno-unused -D__MAC__="64"  -D__USE_XOPEN2K8 -Wall -fno-strict-aliasing -Wno-deprecated $(INC)

CLUSTER=ner_cluster
CLUSTER_SRC=nouns_cluster.cpp strtokenizer.cpp ann.cpp
CLUSTER_OBJ=$(patsubst %.cpp,%.o,$(CLUSTER_SRC))

QUERYCLUSTER=query_cluster
QUERYCLUSTER_SRC=query_cluster.cpp strtokenizer.cpp ann.cpp
QUERYCLUSTER_OBJ=$(patsubst %.cpp,%.o,$(QUERYCLUSTER_SRC))

all : $(QUERYCLUSTER)

%.o:%.cpp
	$(CC) $(CPPFLAGS_CN) -c $< -o $@

#////////////////////////////////////////////////////
$(CLUSTER):$(CLUSTER_OBJ)
	$(CC) -o $@ $^ $(LIBS)
$(QUERYCLUSTER):$(QUERYCLUSTER_OBJ)
	$(CC) -o $@ $^ $(LIBS) -lpthread
#////////////////////////////////////////////////////

.PHONY : clean
clean :
	/bin/rm -f *.o core.*

.PHONY : cleanall
cleanall:
	/bin/rm -f *.o core.* $(CLUSTER) $(QUERYCLUSTER)

