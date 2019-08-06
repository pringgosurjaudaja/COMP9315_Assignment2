// query.c ... query scan functions
// part of Multi-attribute Linear-hashed Files
// Manage creating and using Query objects
// Last modified by John Shepherd, July 2019

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "tuple.h"

// A suggestion ... you can change however you like

struct QueryRep {
	Reln    rel;       // need to remember Relation info
	Bits    known;     // the hash value from MAH
	Bits    unknown;   // the unknown bits from MAH
	PageID  curpage;   // current page in scan
	int     is_ovflow; // are we in the overflow pages?
	Offset  curtup;    // offset of current tuple within page
	int curtuppage;
	//TODO
};

// take a query string (e.g. "1234,?,abc,?")
// set up a QueryRep object for the scan

Query startQuery(Reln r, char *q)
{
	Query new = malloc(sizeof(struct QueryRep));
	assert(new != NULL);

		// TODO
	// Partial algorithm:
	// form known bits from known attributes
	// form unknown bits from '?' attributes
	// compute PageID of first page
	//   using known bits and first "unknown" value
	// set all values in QueryRep object

	//added by Swarnava
	Count nvals = nattrs(r);
	int nstars = 0;
	char *q0 = q;
	int a,b;
	//Tuple t = q;
	//Bits tuple_hash = tupleHash(r,t);
	Bits composite_hash = 0,not_known = 0, oneBit;
	ChVecItem *cv = chvec(r);

	char **attris = malloc(nvals * sizeof(char *));
	int i = 0;
	for(;;){
	  while(*q != ',' && *q != '\0') q++;
	  if(*q == '\0'){
	    //end of query string; add last field to attris
	    attris[i++] = copyString(q0);
	    break;
	  }
	  else{
	    //end of next field; add to attris
	    *q = '\0';
	    attris[i++] = copyString(q0);
	    *q = ',';
	    q++; q0 = q;
	  }
	}

	Bits h[nvals + 1];
	for(i = 0;i < nvals; i++){
	  h[i] = hash_any((unsigned char *)attris[i],strlen(attris[i]));
	  char buff[MAXBITS + 1];
	  bitsString(h[i],buff);
	   printf("%s\n",buff);
	   //printf("%s",attris[i]);
	}


	for(i = 0;i < nvals; i++){
	  if(*attris[i] != '?'){
	    //set d[i] bits in composite hash
	    for(int j =0;j < MAXBITS; j++){
	      a = cv[j].att;
	      b = cv[j].bit;
	      if(a == i){
		oneBit = bitIsSet(h[a],b);
		//printf("%d value \n",j);
		composite_hash = composite_hash | (oneBit<<j);
	      //bit_count++;
	      }
	    }
	    //bit_count = bit_count + depth(r);
	  }

	  else{
	    //for(int k = bit_count;k < bit_count + depth(r);k++){
	    for(int k = 0; k < MAXBITS; k++){
	      a = cv[k].att;
	      b = cv[k].bit;
	      if(a == i){
		not_known = not_known |(1 << k);
		nstars++;
	      }
	      //else{
	      //oneBit = bitIsSet(h[a],b);
	      //composite_hash = composite_hash |  (oneBit << k);

	      //}
	    }
	    //bit_count = bit_count + depth(r);

	  }


	  // printf("%s",attris[i]);
	}
	char buffer[MAXBITS];
	bitsString(composite_hash,buffer);
	//printf("Bit COUUUNT %d\n",bit_count);
	printf("KNOWN %s\n",buffer);

	char uknown[MAXBITS];
	bitsString(not_known,uknown);
	printf("UKNOWN %s\n",uknown);

	printChVec(chvec(r));

	new->known = composite_hash;
	new->unknown = not_known;
	new->rel = r;
	new->curpage = 0;
	new->curtuppage=0;
	//find first page
	//char unknown[MAXBITS];
	//int value_of_bit;
	//bitsString(new->unknown, unknown);
	//for(i = 0; i < depth(r), i++){
	//
	//}

	new->is_ovflow = 0;
	new->curtup = 0;



	return new;
}

// get next tuple during a scan

Tuple getNextTuple(Query q)
{
	// TODO
	// Partial algorithm:
	// if (more tuples in current page)
	//    get next matching tuple from current page
	// else if (current page has overflow)
	//    move to overflow page
	//    grab first matching tuple from page
	// else
	//    move to "next" bucket
	//    grab first matching tuple from data page
	// endif
	// if (current page has no matching tuples)
	//    go to next page (try again)
	// endif
	int x=0;
	//int i = q->curtuppage;
	int z =0;
	int offset = q->curtup;
	Page p =  getPage(dataFile(q->rel),q->curpage);
	Tuple t[MAXTUPLEN];
	Tuple comp[MAXTUPLEN];
	if(q->curtup <= p->ntuples){
		while (q->curtuppage<=p->ntuples) {
			z==0;
			while(p->data[offset]!='\0'){
				comp[z] = p->data[offset];
				offset++;
				z++;
			}
			if(tupleMatch(t,q->known)&& tupleMatch(t,q->unknown)){
				q->curtup = offset;
				return t;
			}
			q->curtuppage++;
		}
	}
	else if(pageOvflow(p)!= NO_PAGE){

	}
	else{
		//go next page
		q->curpage++;
		q->curtuppage=0;
		q->curtup=0;
		p = getPage(dataFile(q->rel),q->curpage);
		offset = 0;
		while (q->curtuppage<=p->ntuples) {
			z==0;
			while(p->data[offset]!='\0'){
				comp[z] = p->data[offset];
				offset++;
				z++;
			}
			if(tupleMatch(t,q->known)&& tupleMatch(t,q->unknown)){
				q->curtup = offset;
				return t;
			}
			q->curtuppage++;
		}
	}
	return NULL;
}

// clean up a QueryRep object and associated data

void closeQuery(Query q)
{
	free(q);
	// TODO
}
