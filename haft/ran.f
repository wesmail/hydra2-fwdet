      real*4 function ran(idum)
      implicit none
      integer*4 idum, IA, IM, IQ, IR, NTAB, NDIV
      real*4 AM, EPS, RNMX
      parameter (IA=16807, IM=2147483647, AM=1./IM,IQ=127773, IR=2836,
     +           NTAB=32, NDIV=1+(IM-1)/NTAB, EPS=1.2e-7, RNMX=1.-EPS)
c
c  Random number generator of Park and Miller with BAYS-DURHAM shuffle
c  and added safeguards.
c  Returns a uniform random deviate between 0 and 1 (exclusive of the
c  end points)
c  (From "Numerical recipes in Fortran", by William H. Press
c                                         Saul A. Teukolsky
c                                         William T. Vetterling
c                                         Brian P. Flannery
c             Cambridge University Press)
c
      integer*4 j, k, iv(NTAB), iy
      save iv, iy
      data iv /NTAB*0/, iy /0/
c
      if (idum.le.0 .or. iy.eq.0) then
         idum = max(-idum,1)
         do j=NTAB+8,1,-1
            k = idum/IQ
            idum = IA*(idum-k*IQ)-IR*k
            if (idum.lt.0) idum = idum+IM
            if (j.le.NTAB) iv(j) = idum
         end do
         iy = iv(1)
      end if
      k = idum/IQ
      idum = IA*(idum-k*IQ)-IR*k
      if (idum.lt.0) idum = idum+IM
      j = 1+iy/NDIV
      iy = iv(j)
      iv(j) = idum
      ran = min(AM*iy,RNMX)
      return
      end
