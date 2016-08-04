c  Read and interpolate Hades Acceptance Matrix for Theorists
c
c  Code version 1.1 of April 15, 2008
c
c  Created :  07/04/05 R. Holzmann
c  Modified : 25/04/05 R. Holzmann  added new interpolation modes
c  Modified : 04/05/05 R. Holzmann  added resolution function
c  Modified : 24/06/05 R. Holzmann  added pair acceptance
c  Modified : 20/07/05 R. Holzmann  fixed error messages on input
c  Modified : 06/02/06 R. Holzmann  support for diff. matrix sizes (v1.0) 
c  Modified : 15/04/08 R. Holzmann  added function to set resolution (v1.1) 
c
c  Files needed : readHAFT.inc
c
c  Usage : 1) set acceptance file name with
c                 call setFileName(fname)
c          2) sample single acceptance values with calls to
c                 acc = getHadesAcceptance(id,mom,theta,phi,mode)
c          3) sample pair acceptance values with calls to
c                 acc = getHadesPairAcceptance(mass,pt,rapidity,mode)
c
c
      real*4 function getHadesAcceptance(pid,p0,theta0,phi0,mode)
c
c  Returns HADES acceptance for particle id of given momentum (in GeV/c),
c  polar angle theta (in deg.) and azimuthal angle phi (in deg.)
c  by table interpolation.
c
c  Lab frame used: z = beam axis, y = vertical axis
c
c                 ^ y
c            x \  |
c               \ |
c                \|    z
c                 O---->
c
c  id = 2 : positron
c     = 3 : electron
c     = 8 : pi+
c     = 9 : pi-
c     = 10: K+
c     = 12: K-
c     = 14: proton
c
c  mode = 0 : nearest-neighbour interpolation
c       = 1 : tri-linear interpolation
c       = 2 : tri-quadratic interpolation
c       =-2 : tri-quadratic B-spline interpolation
c       = 3 : tri-cubic interpolation
c       =-3 : tri-cubic B-spline interpolation
c       = 4 : tri-cubic Catmull-Rom spline
c       =-4 : tri-cubic optimal cardinal spline
c 
      implicit none
      integer*4 pid, mode
      real*4 p0, theta0, phi0
c
      real*4 p, theta, phi
      real*4 u, v, w
      integer*4 ix, iy, iz, i, j, k
      integer*4 ilo, ihi, jlo, jhi, klo, khi
      real*4 sum, Kx, Ky, Kz, kernel
      integer*4 xdim, ydim, zdim
      real*4 plo, pup, dp, thlo, thup, dth, phlo, phup, dph
c
      integer*4 retcode, readHAFTmatrix
      real*4 getMatrixVal
      integer*4 mod
c
c
      mod = -2   ! use B-spline interpolation
c     mod = mode
c
      getHadesAcceptance = 0.

      retcode = readHAFTmatrix()
      if (retcode.eq.-1) return

      call getDimensions(pid,xdim,ydim,zdim)
      call getLimits(pid,plo,pup,dp,thlo,thup,dth,phlo,phup,dph) 

      if (p0.lt.plo .or. theta0.lt.thlo .or. theta0.gt.thup) return
      if (phi0.lt.phlo) return
c
      p = amin1(p0,pup-2.01*dp)  ! level off acceptance at high p
      theta = theta0
      phi = phi0
      if (phi. gt. 60.) phi = amod(phi,60.)   ! modulo HADES sector
c
      ix = xdim*((p-0.5*dp-plo)/(pup-plo)) + 1      ! floor indexes
      iy = ydim*((theta-0.5*dth-thlo)/(thup-thlo)) + 1
      iz = zdim*((phi-0.5*dph-phlo)/(phup-phlo)) + 1

      if (mod.eq.0 .or. mod.eq.1) then   ! set summation limits
        ilo = ix
        ihi = ix+1
        jlo = iy
        jhi = iy+1
        klo = iz
        khi = iz+1
      else if (abs(mod).eq.2 .or. abs(mod).eq.3
     +                       .or. abs(mod).eq.4) then
        ilo = ix-1
        ihi = ix+2
        jlo = iy-1
        jhi = iy+2
        klo = iz-1
        khi = iz+2
      else  ! mode not defined
        return
      end if
c
      if (ilo.lt.0 .or. jlo.lt.0 .or. klo.lt.0) return
      if (ihi.gt.xdim+1 .or. jhi.gt.ydim+1 .or. khi.gt.zdim+1) return
c
      sum = 0. 
      do i=ilo,ihi                      ! triple interpolation loop
        u = (p - (real(i)-0.5)*dp-plo)/dp
        Kx = kernel(u,mod)
        do j=jlo,jhi
          v = (theta - (real(j)-0.5)*dth-thlo)/dth
          Ky = kernel(v,mod)
          do k=klo,khi
            w = (phi - (real(k)-0.5)*dph-phlo)/dph
            Kz = kernel(w,mod)
            sum = sum + getMatrixVal(i,j,k,pid)*Kx*Ky*Kz
          end do
        end do
      end do
      sum = amax1(amin1(sum, 1.01),0.0)  ! clip over/undershoots
c
      getHadesAcceptance = sum
c
      return
      end
c
c
c
      real*4 function getHadesPairAcceptance(mass0,pt0,rap0,mode)
c
c  Returns HADES pair acceptance for given mass (in GeV/c**2),
c  transverse momentum (in GeV/c) and rapidity (in lab frame)
c  by table interpolation.
c
c  Lab frame used: z = beam axis, y = vertical axis
c
c                 ^ y
c            x \  |
c               \ |
c                \|    z
c                 O---->
c
c
c  mode = 0 : nearest-neighbour interpolation
c       = 1 : tri-linear interpolation
c       = 2 : tri-quadratic interpolation
c       =-2 : tri-quadratic B-spline interpolation
c       = 3 : tri-cubic interpolation
c       =-3 : tri-cubic B-spline interpolation
c       = 4 : tri-cubic Catmull-Rom spline
c       =-4 : tri-cubic optimal cardinal spline
c 
      implicit none
      integer*4 mode
      real*4 mass0, pt0, rap0
c
      real*4 mass, pt, rap
      real*4 u, v, w
      integer*4 ix, iy, iz, i, j, k
      integer*4 ilo, ihi, jlo, jhi, klo, khi
      real*4 sum, Kx, Ky, Kz, kernel
      integer*4 xdim, ydim, zdim
      real*4 mlo, mup, dm, ptlo, ptup, dpt, raplo, rapup, drap
c
      logical executed
      data executed /.false./
      save executed
      integer*4 retcode, readHAFTPairmatrix
      real*4 getMatrixVal
      integer*4 mod
c
      retcode = readHAFTPairMatrix()
      if (retcode.eq.-1) return
c
      mod = 1    ! use trilinear interpolation
c     mod = mode ! (use mode = 0 or 1, otherwise problems at pt=0!) 
c
      call getDimensions(51,xdim,ydim,zdim)
      call getLimits(51,mlo,mup,dm,ptlo,ptup,dpt,raplo,rapup,drap) 

      getHadesPairAcceptance = 0.
      if (mass0.lt.mlo .or. pt0.lt.ptlo .or. pt0.gt.ptup
     +    .or. rap0.lt.raplo .or. rap0.gt.rapup) return
c
      mass = amin1(mass0,mup-2.01*dm)  ! level off acceptance at high mass
      pt = pt0
      rap = rap0
c
      ix = xdim*((mass-0.5*dm-mlo)/(mup-mlo)) + 1      ! floor indexes
      iy = ydim*((pt-0.5*dpt-ptlo)/(ptup-ptlo)) + 1
      iz = zdim*((rap-0.5*drap-raplo)/(rapup-raplo)) + 1

      if (mod.eq.0 .or. mod.eq.1) then   ! set summation limits
        ilo = ix
        ihi = ix+1
        jlo = iy
        jhi = iy+1
        klo = iz
        khi = iz+1
      else if (abs(mod).eq.2 .or. abs(mod).eq.3
     +                       .or. abs(mod).eq.4) then
        ilo = ix-1
        ihi = ix+2
        jlo = iy-1
        jhi = iy+2
        klo = iz-1
        khi = iz+2
      else  ! mode not defined
        return
      end if
c
      if (ilo.lt.0 .or. jlo.lt.0 .or. klo.lt.0) return
      if (ihi.gt.xdim+1 .or. jhi.gt.ydim+1 .or. khi.gt.zdim+1) return
c
      sum = 0. 
      do i=ilo,ihi                      ! triple interpolation loop
        u = (mass - (real(i)-0.5)*dm-mlo)/dm
        Kx = kernel(u,mod)
        do j=jlo,jhi
          v = (pt - (real(j)-0.5)*dpt-ptlo)/dpt
          Ky = kernel(v,mod)
          do k=klo,khi
            w = (rap - (real(k)-0.5)*drap-raplo)/drap
            Kz = kernel(w,mod)
            sum = sum + getMatrixVal(i,j,k,51)*Kx*Ky*Kz
          end do
        end do
      end do
      sum = amax1(amin1(sum, 1.01),0.0)  ! clip over/undershoots
c
      getHadesPairAcceptance = sum
c
      return
      end
c
c
c
      real*4 function kernel(u,mode)
c
c  Compute interpolation kernel
c
c  mode = 0: nearest neighbour
c       = 1: piece-wise linear
c       = 2: piece-wise quadratic
c       =-2: quadratic B-spline
c       = 3: cubic spline (B=0,C=1)
c       =-3: cubic B-spline (B=1, C=0)
c       = 4: cubic Catmull-Rom spline (B=0, C=1/2)
c       =-4: cubic "optimal" cardinal spline (B=1/3, C=1/3)
c
c  mode >=0: interpolating (i.e. exact at grid points)
c  mode < 0: approximating (i.e. not exact, but smoother)
c

      implicit none
      real*4 u
      integer*4 mode
      real*4 ua
c
      if (mode.eq.0) then       ! nearest neighbour
        if (u.gt.-0.5 .and. u.le.0.5) then
          kernel = 1.
        else 
          kernel = 0.
        end if
        return
c
      else if (mode.eq.1) then  ! linear
        ua = abs(u)
        if (ua.lt.1.) then
          kernel = 1.-ua
        else 
          kernel = 0.
        end if
        return
c
      else if (mode.eq.2) then  ! quadratic
        ua = abs(u)
        if (ua.le.0.5) then
          kernel = 1.0-2.*ua*ua
        else if (ua.le.1.5) then
          kernel = 1.5+(ua-2.5)*ua
        else 
          kernel = 0.
        end if
        return
c
      else if (mode.eq.-2) then ! quadratic B-spline
        ua = abs(u)
        if (ua.le.0.5) then
          kernel = 0.75-ua*ua
        else if (ua.le.1.5) then
          kernel = 1.125+0.5*(ua-3.)*ua
        else 
          kernel = 0.
        end if
        return
c
      else if (mode.eq.3) then  ! cubic
        ua = abs(u)
        if (ua.le.1.) then
          kernel = 1.+(ua-2.)*ua*ua
        else if (ua.le.2.) then
          kernel = 4.+((5.-ua)*ua-8.)*ua
        else 
          kernel = 0.
        end if
        return
c
      else if (mode.eq.-3) then ! cubic B-spline
        ua = abs(u)
        if (ua.le.1.) then
          kernel = 2./3.+(0.5*ua-1.)*ua*ua
        else if (ua.le.2.) then
          kernel = 4./3.+((1.-ua/6.)*ua-2.)*ua
        else 
          kernel = 0.
        end if
        return
c
      else if (mode.eq.4) then  ! cubic Catmull-Rom
        ua = abs(u)
        if (ua.le.1.) then
          kernel = 1.+(1.5*ua-2.5)*ua*ua
        else if (ua.le.2.) then
          kernel = 2.+((2.5-0.5*ua)*ua-4.)*ua
        else 
          kernel = 0.
        end if
        return
c
      else if (mode.eq.-4) then ! optimal cubic cardinal spline (=compromise
                                ! between blurring and ringing)
        ua = abs(u)
        if (ua.le.1.) then
          kernel = 8./9.+(7./6.*ua-2.)*ua*ua
        else if (ua.le.2.) then
          kernel = 16./9.+((2.-7./18.*ua)*ua-10./3.)*ua
        else 
          kernel = 0.
        end if
        return
c
      else                      ! undefined mode
        kernel = 0.
        return
      end if
c
      end
c
c
c
      integer*4 function readHAFTmatrix()
c
c  Opens file in unformatted direct access mode
c  and reads HADES acceptance matrices (as linearized arrays)
c
      implicit none
c
      include 'readHAFT.inc'
c
      integer*4 runit
      parameter (runit=77)  ! change if input unit is already busy
      integer*4 pid
      integer*4 i, j, k, n
      integer*4 bytes       ! byte counter
      integer*4 bins
      integer*4 lc          ! string length
c
      readHAFTmatrix = 0
c
      if (readflag.eq.1) return
c
      readflag = 0
      do i=1,size           ! set all matrices to 0
        matrix2(i) = 0.
        matrix3(i) = 0.
        matrix8(i) = 0.
        matrix9(i) = 0. 
        matrix10(i) = 0.
        matrix12(i) = 0.
        matrix14(i) = 0.
      end do
      do i=1,nids
        xdim(i) = 0
        ydim(i) = 0
        zdim(i) = 0
        pmin(i) = 0.
        pmax(i) = 0.
        dp(i) = 0.
        thmin(i) = 0.
        thmax(i) = 0.
        dth(i) = 0.
        phmin(i) = 0.
        phmax(i) = 0.
        dph(i) = 0.
      end do
c
c
      lc = len(fname)
      do i=1,lc
         if (fname(i:i+3).eq.'.acc') goto 1
      end do
 1    lc = i+3
      open(unit=runit, file=fname, form='unformatted', access='direct',
     +     status='old',recl=1, err=99)  ! g77 on linux
c     open(unit=runit, file=fname, form='binary', access='direct',
c    +     status='old',recl=1, err=99)  ! Compaq f90 on MS Windows
      bytes=1
      write(6,*) ' '
      read(runit,rec=bytes,err=100) comment
      write(6,'(a80)') comment
      write(6,*) '--------------------------------------'
      bytes = bytes + 80
      read(runit,rec=bytes,err=100) sigpA(1), sigpA(2), sigpA(3),
     +                              sigpB(1), sigpB(2), sigpB(3),
     +                              sigth, sigph, XX0
      bytes = bytes + 9*4

 40   read(runit,rec=bytes,err=50) pid  ! break out if EOF reached
      if (pid.lt.1 .or. pid.gt.nids) goto 49
      bytes = bytes + 4
      read(runit,rec=bytes,err=100) xdim(pid), ydim(pid), zdim(pid)
c
      bins = xdim(pid)*ydim(pid)*zdim(pid)
      if (bins.gt.size) goto 101  ! check if enough memory available
c
      bytes = bytes + 3*4
      read(runit,rec=bytes,err=100) pmin(pid),pmax(pid),
     +                              thmin(pid),thmax(pid),
     +                              phmin(pid),phmax(pid)
      bytes = bytes + 6*4
 49   if (pid.eq.2) then        ! positron
        read(runit,rec=bytes,err=100) (matrix2(i),i=1,bins)
        write(6,'(''Matrix read for e+'')')
      else if (pid.eq.3) then     ! electron
        read(runit,rec=bytes,err=100) (matrix3(i),i=1,bins)
        write(6,'(''Matrix read for e-'')')
      else if (pid.eq.8) then     ! pi+
        read(runit,rec=bytes,err=100) (matrix8(i),i=1,bins)
        write(6,'(''Matrix read for pi+'')')
      else if (pid.eq.9) then     ! pi-
        read(runit,rec=bytes,err=100) (matrix9(i),i=1,bins)
        write(6,'(''Matrix read for pi-'')')
      else if (pid.eq.10) then    ! K+
        read(runit,rec=bytes,err=100) (matrix10(i),i=1,bins)
        write(6,'(''Matrix read for K+'')')
      else if (pid.eq.12) then    ! K-
        read(runit,rec=bytes,err=100) (matrix12(i),i=1,bins)
        write(6,'(''Matrix read for K-'')')
      else if (pid.eq.14) then    ! proton
        read(runit,rec=bytes,err=100) (matrix14(i),i=1,bins)
        write(6,'(''Matrix read for p'')')
      else
        write(6,'(''Unsupported particle id: '',I3,''  STOP!'')')
        stop
      end if
      bytes = bytes + bins*4

c     write(6,*) 'Matrix for ID= ',pid
      write(6,*) 'dims= ',xdim(pid), ' ', ydim(pid), ' ', zdim(pid)
      write(6,*) 'lims= ',pmin(pid), ' ', pmax(pid), ' ', thmin(pid),
     +           ' ', thmax(pid), ' ', phmin(pid), ' ', phmax(pid)
      write(6,*) 'size of matrix :', bins
      write(6,*) '--------------------------------------'
      dp(pid) = (pmax(pid)-pmin(pid))/real(xdim(pid))
      dth(pid) = (thmax(pid)-thmin(pid))/real(ydim(pid))
      dph(pid) = (phmax(pid)-phmin(pid))/real(zdim(pid))

      goto 40  ! loop until EOF is reached
 50   close(runit)
c
c
c
      readHAFTmatrix = bytes-1 ! return number of bytes read
      readflag = 1
      return
c
c     Error opening or reading
c
 99   close(runit)
      write(6,*) 'Open error on unit ', runit, ' File = ',fname(1:lc)
      readHAFTMatrix = -1
      return
 100  close(runit)
      write(6,*) 'Read error on unit ', runit, ' File = ',fname(1:lc)
      readHAFTmatrix = -1
      return
 101  close(runit)
      write(6,*) 'Size error: ', bins, ' >', size,' File = ',fname(1:lc)
      readHAFTmatrix = -1
      return
      end
c
c
c
      integer*4 function readHAFTPairMatrix()
c
c  Opens file in unformatted direct access mode
c  and reads HADES pair acceptance matrix (as linearized array)
c
      implicit none
c
      include 'readHAFT.inc'
c
      integer*4 runit
      parameter (runit=78)  ! change if input unit is already busy
      integer*4 i, j, k, n
      integer*4 bytes       ! byte counter
      integer*4 bins
      integer*4 lc
c
c
      readHAFTPairMatrix = 0
c
      if (readflag2.eq.1) return
c
      readflag2 = 0
      do i=1,size           ! set matrix to 0
        matrix51(i) = 0.
      end do
c
c
      lc = len(fname2)
      do i=1,lc
         if (fname2(i:i+3).eq.'.acc') goto 1
      end do
 1    lc = i+3
      open(unit=runit, file=fname2, form='unformatted', access='direct',
     +     status='old',recl=1, err=99)  ! g77 on linux
c     open(unit=runit, file=fname2, form='binary', access='direct',
c    +     status='old',recl=1, err=99)  ! Compaq f90 on MS Windows
      bytes=1
      read(runit,rec=bytes,err=100) comment2
      write(6,'(a80)') comment2
      bytes = bytes + 80
      read(runit,rec=bytes,err=100) xdim2, ydim2, zdim2
c
      bins = xdim2*ydim2*zdim2
      if (bins.gt.size) goto 101  ! check if enough memory available
c
      bytes = bytes + 3*4
      read(runit,rec=bytes,err=100) mmin,mmax,ptmin,ptmax,rapmin,rapmax
      bytes = bytes + 6*4
      read(runit,rec=bytes,err=100) (matrix51(i),i=1,bins)
      write(6,'(''Matrix read for e+e- pairs'')')
      bytes = bytes + bins*4
      close(runit)
c
      dm = (mmax-mmin)/real(xdim2)
      dpt = (ptmax-ptmin)/real(ydim2)
      drap = (rapmax-rapmin)/real(zdim2)
c
      write(6,'('' coms= '',a80)') comment2
      write(6,*) 'dims= ',xdim2, ' ', ydim2, ' ', zdim2
      write(6,*) 'lims= ',mmin, ' ', mmax, ' ', ptmin, ' ', ptmax,
     +           ' ', rapmin, ' ', rapmax
      write(6,*) 'size of matrix :', bins
c
      readHAFTPairMatrix = bytes-1 ! return number of bytes read
      readflag2 = 1
      return
c
c     Error opening or reading
c
 99   close(runit)
      write(6,*) 'Open error on unit ', runit, ' File = ',fname2(1:lc)
      readHAFTPairMatrix = -1
      return
 100  close(runit)
      write(6,*) 'Read error on unit ', runit, ' File = ',fname2(1:lc)
      readHAFTPairMatrix = -1
      return
 101  close(runit)
      write(6,*) 'Size error: ', bins,' >',size, ' File = ',fname2(1:lc)
      readHAFTPairMatrix = -1
      return
      end
c
c
c
      subroutine setFileName(name)
c
c  Sets name of input file containing the filter
c
      implicit none
      character*(*) name
c
      include 'readHAFT.inc'
      integer*4 dummy, readHAFTmatrix

      fname = name
      dummy = readHAFTmatrix()

c      write(6,'(''name  |'',a80,''|'')') name
c      write(6,'(''fname |'',a80,''|'')') fname

      return
      end
c
c
c
      subroutine setPairFileName(name)
c
c  Sets name of input file containing the filter
c
      implicit none
      character*(*) name
c
      include 'readHAFT.inc'
      integer*4 dummy, readHAFTPairMatrix

      fname2 = name
      dummy = readHAFTPairMatrix()

c      write(6,'(''name  |'',a80,''|'')') name
c      write(6,'(''fname2 |'',a80,''|'')') fname2

      return
      end
c
c
c
      real*4 function getMatrixVal(i,j,k,pid)
c
c  Returns acceptance value at cell (i,j,k) of linearized matrix
c  for particle id
c
      implicit none
      integer*4 i, j, k, pid
c
      include 'readHAFT.inc'
c
      integer*4 xdi, ydi, zdi
      integer*4 i1, j1, k1, ilin
c
      call getDimensions(pid,xdi,ydi,zdi)

      i1 = min(max(1,i),xdi)  ! Make sure indexes stay within table.
      j1 = min(max(1,j),ydi)  ! This effectively extrapolates matrix
      k1 = min(max(1,k),zdi)  ! beyond table boundaries.
c
      ilin = i1+xdi*(j1-1)+xdi*ydi*(k1-1)  ! linearized index
      if (pid.eq.2) then          ! positron
        getMatrixVal = matrix2(ilin)
        return
      else if (pid.eq.3) then     ! electron
        getMatrixVal = matrix3(ilin)
        return
      else if (pid.eq.8) then     ! pi+
        getMatrixVal = matrix8(ilin)
        return
      else if (pid.eq.9) then     ! pi-
        getMatrixVal = matrix9(ilin)
        return
      else if (pid.eq.10) then    ! K+
        getMatrixVal = matrix10(ilin)
        return
      else if (pid.eq.12) then    ! K-
        getMatrixVal = matrix12(ilin)
        return
      else if (pid.eq.14) then    ! proton
        getMatrixVal = matrix14(ilin)
        return
      else if (pid.eq.51) then    ! dilepton
        getMatrixVal = matrix51(ilin)
        return
      else
        getMatrixVal = 0.
        return
      end if
      end
c
c
c
      subroutine getLimits(pid,xlo,xhi,dx,ylo,yhi,dy,zlo,zhi,dz) 
c
c  Return the lower and upper limits, and step sizes of the table
c
      implicit none
      integer*4 pid
      real*4 xlo, xhi, dx, ylo, yhi, dy, zlo, zhi, dz
c
      include 'readHAFT.inc'
c
      if (pid.eq.51) then  ! pair
        xlo  = mmin
        xhi  = mmax
        dx   = dm
        ylo = ptmin
        yhi = ptmax
        dy  = dpt
        zlo = rapmin
        zhi = rapmax
        dz = drap
        return
      else
        xlo  = pmin(pid)
        xhi  = pmax(pid)
        dx   = dp(pid)
        ylo = thmin(pid)
        yhi = thmax(pid)
        dy  = dth(pid)
        zlo = phmin(pid)
        zhi = phmax(pid)
        dz = dph(pid)
        return
      end if
      end
c
c
c
      subroutine getDimensions(pid,nx,ny,nz)
c
c  Return the dimensions of a table of particle pid
c
      implicit none
      integer*4 pid, nx, ny, nz
c
      include 'readHAFT.inc'
c
      if (pid.eq.51) then  ! pair
        nx  = xdim2
        ny = ydim2
        nz = zdim2
      else
        nx = xdim(pid)
        ny = ydim(pid)
        nz = zdim(pid)
      end if
      return
      end
c
c
c
      subroutine smearHades4Momentum(mom,mode,pid)
c
c  Apply the Hades momentum resolution to a 4-momentum vector (in GeV/c)
c
c  Lab frame used: z = beam axis, y = vertical axis
c
c                 ^ y
c            x \  |
c               \ |
c                \|    z
c                 O---->
c
c  All components of the 4-momentum vector are changed by this call.
c
c  The resolution mode is determined by:
c
c   mode = 1 : low-resolution     (MDC 1+2)
c        = 2 : medium-resolution  (MDC 1+2+3)
c        = 3 : high-resolution    (MDC 1+2+3+4)
c
c
      implicit none
      real*4 mom(4)
      integer*4 mode,pid
c
      include 'readHAFT.inc'
c
      integer*4 retcode, readHAFTmatrix
      real*4 mass2, pt, pt2, ptot, ptot2, Etot, theta, phi, sinth
      real*4 sigp, sampleGauss, betainv, sigms, sigms2, sigthms, sigphms
      real*4 pi, twopi
      parameter (pi=3.141592654, twopi=2.*pi)
c
      if (mode.lt.1. .or. mode. gt.3) return ! unknown mode
c
      if (readflag.eq.0) then
        retcode = readHAFTmatrix()
        if (retcode.eq.-1) return
      end if
c
      pt2 = mom(1)**2 + mom(2)**2
      pt = sqrt(pt2)
      ptot2 = pt2 + mom(3)**2
      ptot = sqrt(ptot2)                      ! total momentum in GeV/c
      mass2 = mom(4)**2 - ptot2               ! particle mass does not change
      if (mass2.lt.0.) mass2 = 0.
      betainv = 1.
      if (mass2.gt.0.) betainv = sqrt(1. + mass2/ptot2)  ! 1/beta
      sigms = 0.0136*betainv/ptot*sqrt(XX0)*(1+0.038*log(XX0))
                                              ! multiple scattering angle
      sigms2 =sigms*sigms
      sigthms = sqrt(sigth*sigth+sigms2)      ! add quadratically to resolution
      sigphms = sqrt(sigph*sigph+sigms2)
c
      theta = acos(mom(3)/ptot)               ! polar angle in lab frame
      sinth = sin(theta)
      phi = 0.
      if (pt.gt.0.) phi = acos(mom(1)/pt)
      if (mom(2).lt.0.) phi = twopi - phi     ! azimuthal angle in lab frame
c
      sigp = 0.01*ptot*(sigpA(mode)+sigpB(mode)*ptot) ! momentum resolution
      ptot = amax1(0.,sampleGauss(ptot,sigp)) ! smear total momentum
      theta = abs(sampleGauss(theta,sigthms)) ! smear polar angle
      if (theta.gt.pi) theta = twopi - theta  ! if > pi, mirror angle
      if (sinth.gt.0.01) phi = sampleGauss(phi,sigphms/sinth) ! smear azimuth
      if (phi.lt.0.) phi = phi + twopi        ! and check if within range 
      if (phi.gt.twopi) phi = phi - twopi
c
      sinth = sin(theta)
      mom(1) = ptot*sinth*cos(phi)            ! new momentum components
      mom(2) = ptot*sinth*sin(phi)
      mom(3) = ptot*cos(theta)
      mom(4) = sqrt(mom(1)**2 + mom(2)**2 + mom(3)**2 + mass2)  ! total energy
c
      return
      end
c
c
c
      subroutine smearHadesMomentum(px,py,pz,etot,mode,pid)
      real*4 px, py, pz, etot
      integer* 4 mode, pid
      real*4 mom4(4)
      mom4(1) = px
      mom4(2) = py
      mom4(3) = pz
      mom4(4) = etot
      call smearHades4Momentum(mom4,mode,pid)
      px = mom4(1)
      py = mom4(2)
      pz = mom4(3)
      etot = mom4(4)
      return
      end
c
c
c
      subroutine smearHades3Momentum(mom3,mode,pid)
c
c  Apply Hades momentum resolution to a 3-momentum (calculate multiple
c  scattering assuming the particle is an electron) 
c
      implicit none
      real*4 mom3(3)
      integer*4 mode,pid
      real*4 mom4(4)
      real*4 mass
c
      if (mode.lt.1 .or. mode.gt.3) return    ! unknown mode
c
      if (pid.eq.2 .or. pid.eq.3) then        ! e+ or e-
         mass = 0.000511
      else if (pid.eq.8 .or. pid.eq.9) then   ! pi+ or pi- 
         mass = 0.139570
      else if (pid.eq.11 .or. pid.eq.12) then ! K+ or K- 
         mass = 0.493677
      else if (pid.eq.14) then                ! proton
         mass = 0.938272
      else
         return  ! particle is not supported
      end if

      mom4(1) = mom3(1)
      mom4(2) = mom3(2)
      mom4(3) = mom3(3)
      mom4(4) = sqrt(mom3(1)**2+mom3(2)**2+mom3(3)**2 + mass**2)
      call smearHades4Momentum(mom4,mode,pid)
      mom3(1) = mom4(1)
      mom3(2) = mom4(2)
      mom3(3) = mom4(3)
      return
      end
c
c
c
      subroutine smearHadesPair(pair,mode)
c
c  Apply Hades momentum resolution to a pair (calculate multiple
c  scattering assuming the particle is an electron) 
c
      implicit none
      real*4 pair(3)
      integer*4 mode
c
      include 'readHAFT.inc'
c
      real*4 m, pt, rap, sigpt, sigm, sigrap
      real*4 sampleGauss
      integer*4 retcode, readHAFTmatrix
c
      if (readflag.eq.0) then
        retcode = readHAFTmatrix()
        if (retcode.eq.-1) return
      end if
c
      m = pair(1)
      pt = pair(2)
      rap = pair(3)

      sigpt = 0.01*pt*(sigpA(mode)+sigpB(mode)*pt)/sqrt(2.) ! pt resolution
      pt = amax1(0.,sampleGauss(pt,sigpt))                  ! smear pt

      sigm = 0.01*m*(sigpA(mode)+sigpB(mode)*m)/sqrt(2.)
      m = amax1(0.,sampleGauss(m,sigm))                     ! smear mass

      sigrap = 0.1    !  just a guess!
      rap = sampleGauss(rap,sigrap)

      pair(1) = m
      pair(2) = pt
      pair(3) = rap
      return
      end
c
c
c

      real*4 function sampleGauss(mean,sigma)
c
c  Return random number according to a normal distribution.
c
c  Calls ran(iseed), a uniform random number generator returning ]0,1[.
c
      implicit none
c
      include 'readHAFT.inc'
c
      real*4 mean, sigma
      real*4 pi, twopi
      parameter (pi=3.141592654, twopi=2.*pi)
      real*4 theta, ran
c
      sampleGauss = mean
      if (sigma.le.0.) return
      theta = twopi*ran(iseed)
      sampleGauss = mean + sigma*cos(theta)*sqrt(-2.*alog(ran(iseed)))
      return
      end
c
c
c
      subroutine setResolutionParameters(mode,a,b)
c
c     Set momentum resolution parameters
c
      implicit none
c
      include 'readHAFT.inc'

      real*4 a, b
      integer*4 mode

      if (mode.ge.1 .and. mode.le.3) then
         sigpA(mode) = a
         sigpB(mode) = b
      endif

      write(6,*) 'mode=1 ', ' ', sigpA(1), ' ', sigpB(1)
      write(6,*) 'mode=2 ', ' ', sigpA(2), ' ', sigpB(2)
      write(6,*) 'mode=3 ', ' ', sigpA(3), ' ', sigpB(3)
      return
      end
c
c
c
      block data blockHAFT
c
c  Needed to provide a default file name and random number seed
c
      implicit none
      include 'readHAFT.inc'
      data fname /'HadesAcceptanceFilter.acc'/
      data fname2 /'HadesPairAcceptanceFilter.acc'/
      data iseed /123/
      data readflag /0/
      data readflag2 /0/
      end
