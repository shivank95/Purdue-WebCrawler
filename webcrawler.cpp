#include "webcrawler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


// Add your implementation here

	//Constructor
WebCrawler::WebCrawler(int maxUrls, int nInitialURls,  const char ** initialURLs)
{
	int cnt = 0;
  // Allocate space for _urlArray
  _urlArray = (URLRecord *) malloc (maxUrls * sizeof(URLRecord));
  
  //Initialize the HashTable
  _urlToUrlRecord = new HashTableTemplate<int>();
  
  _wordToURLRecordList = new HashTableTemplate<URLRecordList *>();
  
  currentWord = (char *) malloc (100 * sizeof(char));
  currentURL = (char *) malloc (300 * sizeof(char));
  
  //_urlArray = (URLRecord *) new URLRecord[nInitialURls];
  
  // insert the initialURls
  while (cnt < nInitialURls) {
  	(_urlArray + cnt)->_url = (char *) initialURLs[cnt];
  	
  	bool e = _urlToUrlRecord->insertItem((const char *)(_urlArray + cnt)->_url, cnt);
  	
  	cnt++;
  }
  
  //FOR TESTING
  /*for (int i = 0; i < nInitialURls; i++) {
  	printf("urlArray: %s\n", (_urlArray + i)->_url);
  }*/
  
  // Update _maxUrls, _headURL and _tailURL
  _maxUrls = maxUrls;
  _headURL = 0;
  _tailURL = 1;
  URLcnt = 0;	
  
}

void
printUsage()
{
  fprintf( stderr, "  Usage: webcrawl [-u <maxurls>] url-list\n");
  fprintf( stderr, "  EXAMPLE: ./webcrawler -u 100 http://www.purdue.edu http://www.slashdot.org http://www.cnn.com\n");
}

void
WebCrawler::onContentFound(char c) {
	char * wordCpy = currentWord;
	while (*wordCpy != '\0') {
		wordCpy++;
	}
	/*if (c != ' ' && c != '.' && c != '?' && c!= '!' && c!= ',' && c!= ':' && c!= ';') {
		*wordCpy = c;
		wordCpy++; 
		*wordCpy = '\0';
	}*/
	if (isalpha(c) || isdigit(c)) {		//Add if character is alphanumeric
		*wordCpy = c;
		wordCpy++; 
		*wordCpy = '\0';
	}
	else if (strlen(currentWord) > 1) {		//Character is a space
		//("WORD: %s URL: %s INDEX: %d\n", currentWord, currentURL, _headURL);
		addWords(currentWord);
		*currentWord = '\0';
	}
}

void
WebCrawler::onAnchorFound(char * url) {
	
	
	getLink(url);
}
int main (int argc, char ** argv) {
	
	const char ** initUrls = (const char **) malloc (argc * sizeof(char *));
	int max = 1000;
	int urlCnt = 0;
	//printf("ARGC: %d\n",argc);
	if (argc == 1) {
		
		printUsage();
		exit(1);
	}
	else if(argc == 2) {
		initUrls[0] = argv[1];
		urlCnt++;
		//printf("%s\n", initUrls[0]);
	}
	else if(argc > 2) {
		
		if (!strcmp(argv[1],"-u")) {		//-u option is there and max urls is specified
			max = atoi(argv[2]);
			if (argc == 3) {
				printUsage();
				exit(1);
			}
			
			int cnt = 3;
			while (cnt < argc) {
				
				initUrls[urlCnt] = argv[cnt];
				//printf("%s\n", initUrls[urlCnt]);
				cnt++;
				urlCnt++;
			}
		}
		else {			//No max url specified
			
			int cnt = 1;
			while (cnt < argc) {
				
				initUrls[urlCnt] = argv[cnt];
				//printf("%s\n", initUrls[urlCnt]);
				cnt++;
				urlCnt++;
			}
		}
	}
	//printf("Max URLS %d\n", max);
	WebCrawler crawler (max, urlCnt, initUrls);		//Create Instance
	crawler.crawl();

}
void
WebCrawler::getLink(char * link) {
	
	//FIX link
	bool addLink = true;
	char * linkCpy = strdup(link);
	
		//REMOVE dirty links
	if (strstr(linkCpy, "#") != NULL) {
		addLink = false;
	}
	if((strstr(linkCpy,"https") != NULL)) {
		addLink = false;
	}
	if(_tailURL >= _maxUrls) {
		addLink = false;
	}
		
		//Fix abnormal links
	
	char * goodLink = (char *) malloc (1000 * sizeof(char));
	if(strstr(linkCpy, "http://") == NULL && addLink == true) {	//If "http://" is not present
		
		if (strstr(linkCpy, "//") == NULL) {	//If "//" is not present
			//printf("\nCHECK\n");
			strcpy(goodLink, "http://www.purdue.edu/");		//Add hostname
			strcat(goodLink, (const char *) linkCpy);
			link = strdup(goodLink);
			//printf("GOODSTRING: %s\n", goodLink);
		}
		else {
			//printf("\nCHECK\n");
			strcpy(goodLink, "http://www.");
			strcat(goodLink, (const char *) (linkCpy + 2));
			link = strdup(goodLink);
			//printf("GOODSTRING: %s\n", goodLink);
			
		}
	}
		//free goodLink	
	free(goodLink);
	
	
	linkCpy = link;		//Make sure linkCpy still points to a clean link
	
	if((strstr(linkCpy,"www.purdue.edu") == NULL)) {		//Only Purdue Websites
		addLink = false;
	}
	if((strstr(linkCpy,"/../") != NULL)) {		//Remove bad link
		addLink = false;
	}
	
	//Check if link is present using Hash Table
	int index;
	bool found;
	found = _urlToUrlRecord->find(linkCpy, &index);
	if (found == true) {
		char * current = (_urlArray + index)->_url;
		if (strcmp(current, linkCpy) == 0) {		//If an existing URL is found
			addLink = false;
			//printf("\n\nLink already present");
		}
	}
	
	
	if (addLink == true) {
		(_urlArray + _tailURL)->_url = strdup(link);
		
		//Add url to URL Record Hash Table
		bool e;
		e = _urlToUrlRecord->insertItem((const char *)link, _tailURL);	
		_tailURL++;
		
	}
	
	
	
}
void 
WebCrawler::addWords(char * word) {
	
	
	char * wordCpy = strdup(word);
	URLRecordList * wordRecord;
	int currentIndex = _headURL;
	bool found;
	found = _wordToURLRecordList->find(wordCpy, &wordRecord);
	bool exists = false; //doesnt exist
	if (found == true) {
		
		//Word already exists, add index to linked list
		while(wordRecord->_next != NULL) {
			if(wordRecord->_urlRecordIndex == currentIndex) {
				//index for this URL already exists
				exists = true;
			}
			
			wordRecord = wordRecord->_next;
		}
		
		if (exists == false) {
			//add word to linked list
			URLRecordList * e = new URLRecordList;
			e->_urlRecordIndex = currentIndex;
			e->_next = NULL;
			wordRecord->_next = e;
		}
		
	}
	else {
	
		//Word not found. Add word to Hash Table
		
		URLRecordList * newRecord = new URLRecordList;
		newRecord->_urlRecordIndex = currentIndex;
		//("\nNEW WORD ADDED: %s\n", wordCpy);
		newRecord->_next = NULL;
		bool e;
		e = _wordToURLRecordList->insertItem(wordCpy, newRecord);
		
	}
}
void
WebCrawler::crawl() {

	while (_headURL <_tailURL) {
		
		//Fetch the next URL in _headURL
		int n;
		char * currentUrl = (_urlArray + _headURL)->_url;
		
		//Current URL, used for adding words
		currentURL = strdup(currentUrl);
		
		//Get Descriptions
		int m = 1000;
		char * description = fetchHTML(currentUrl, &m);
		if (description != NULL) {
		  FILE *fp;
		  fp = fopen("descriptions.txt","w+");
		  fwrite(description, 1, 1000, fp);
		  
		  //Beginning of the file
		  fseek(fp, SEEK_SET, 0);
		  
		  *description = '\0';
		  
		  fread(description, 1, 1000,(FILE*) fp);
		  
		  char * offset = description;
		  *(offset + 1000) = '\0';
		  
		  //printf("\n\nMY FILE: \n\n%s\n\n",description);
		  
		  //Find Meta Content
		  char * jumpDescription;
		  
		  char * goodDescription = (char *) malloc (1000 * sizeof(char));
		  bool descriptionAdded = true;
		  if(((jumpDescription = strstr(description,"<meta content=")) != NULL) && (strstr(description, "<meta content=\"$")) == NULL) {		//If Meta content is found
			
			
			//printf("META CONTENT: %s\n", jumpDescription);
			jumpDescription = strstr(jumpDescription,"\"");
			
			if (jumpDescription != NULL) {
				goodDescription = strdup(jumpDescription + 1);
				jumpDescription = strstr(goodDescription, "\"");
				char * descCheck = strdup(jumpDescription + 1);
				char * descCheckEnd = strstr(descCheck, "/>");
				*descCheckEnd = '\0';
				
				
				if (strstr(descCheck, "description") != NULL) {
					//Description has tag "description
				}
				if (jumpDescription != NULL) {
					*jumpDescription = '\0';
					if (strstr(goodDescription, "400 Bad Request") != NULL || strstr(goodDescription, "302 Found") != NULL ||
					strstr(goodDescription, "301 Moved Permanently") != NULL  || strstr(goodDescription, "Page not found") != NULL || strstr			(goodDescription, "403 Forbidden") != NULL) {
						descriptionAdded = false;
					}
					else {
						(_urlArray + _headURL)->_description = strdup(goodDescription);
						printf("\nGOOD DESCRIPTION: %s\n", goodDescription);
						descriptionAdded = true;
					}
				}
				else {
					descriptionAdded = false;
				}
			}
			else {
				descriptionAdded = false;
			}
			
			
			
		  }
		  
		  else if (((jumpDescription = strstr(description, "<title>")) != NULL)) {			//Search for title tag in description
		  	
		  	jumpDescription = jumpDescription + strlen("<title>");
		  	//printf("JUMP DESCRIPTION: %s\n", jumpDescription);
		  	
		  	if (jumpDescription != NULL) {
				goodDescription = strdup(jumpDescription);
				jumpDescription = strstr(goodDescription, "</title>");
				if (jumpDescription != NULL) {
					*jumpDescription = '\0';
					
					if (strstr(goodDescription, "400 Bad Request") != NULL || strstr(goodDescription, "302 Found") != NULL ||
						strstr(goodDescription, "301 Moved Permanently") != NULL || strstr(goodDescription, "Page not found" ) != NULL || 
						strstr(goodDescription, "403 Forbidden") != NULL) {
						descriptionAdded = false;
					}
					else {
						(_urlArray + _headURL)->_description = strdup(goodDescription);
						//("\nGOOD DESCRIPTION: %s\n", goodDescription);
					}
				}
				else {
					descriptionAdded = false;
				}
			}
			else {
				descriptionAdded = false;
			}
		  }	
		  
		  if (descriptionAdded == false || (_urlArray + _headURL)->_description == NULL) {
		  	
		  	(_urlArray + _headURL)->_description = strdup((_urlArray+_headURL)->_url);
		  }
	
		  
		  fclose(fp);
		  free(goodDescription);
		  free(description);
	  	}
		
		//Get hyperlinks
		char * buffer = fetchHTML(currentUrl, &n);
		if (buffer==NULL) {
	  		//continue;
	  	}
	  	else {
	  		
	  		parse(buffer,n);		//Get Links and fill up URL array
		}
		//Concatenate head
		_headURL++;
		
		if (_tailURL == 1000) {
			break;
		}	
	}
		/*printf("\n\nURL ARRAY: \n\n\n");
		
		for (int i = 0; i < _tailURL; i++) {
  			("CHECKER: %s\n %s\n\n", (_urlArray + i)->_url, (_urlArray + i)->_description);
  			
  		}*/	
  		
  		//Create url.txt
  		FILE * urlFile;
  		
  		urlFile = fopen("url.txt","w");
  		
  		for (int i = 0; i < _tailURL; i++) {
  			//print url name
  			fprintf(urlFile, "%d %s\n", (i+1), (_urlArray + i)->_url);
  			//print description
  			fprintf(urlFile, "%s\n\n", (_urlArray + i)->_description);
  		}
		
		fclose(urlFile);
		
		/*
		
  HashTableTemplateIterator<int> iterator(&h);

  int sum2=0;
  const char * key;
  int grade;
  while (iterator.next(key, grade)) {
    sum2+=grade;
    //printf("%s %d\n", key, grade);
  }

  assert(sum2==sum);
  
		*/
		FILE * wordFile;
		
		wordFile = fopen("word.txt","w");
		
		HashTableTemplateIterator<URLRecordList *> iterator(_wordToURLRecordList);
		
		const char * wordKey;
		URLRecordList * tableData;
		
		while (iterator.next(wordKey, tableData)) {
			fprintf(wordFile, "%s ", wordKey);
			while (tableData != NULL) {
				
				fprintf(wordFile, "%d ", tableData->_urlRecordIndex);
				tableData = tableData->_next;
			}
			fprintf(wordFile, "\n");
		}
		
		fclose(wordFile);
}



