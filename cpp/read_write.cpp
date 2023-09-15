void Reader() {
    semWait(mutex)
	readcount := readcount + 1
	if readcount == 1 then semWait(wrt)
	semSignal(mutex)

	/* Reading */

	semWait(mutex)
	readcount := readcount - 1
	if readcount == 0 then semSignal(wrt)
	semSignal(mutex)

}

void Writer() {

	semWait(wrt)
	/* Writing */
	semSignal(wrt)
}