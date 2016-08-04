c  Read HADES unformatted acceptance matrix for theorists
c
c  Created :  07/04/05 R. Holzmann
c  Modified : 10/02/06 R. Holzmann
c
      program testHAFT
c
      implicit none
c
      include 'readHAFT.inc'
c
      integer*4 ix,iy,iz
      real*4 p,theta,phi,val,getHadesAcceptance
      integer*4 i,j,k, id, flag
      real*4 pv,thv,phv
c
      call setFileName('HadesAcceptanceFilterRaw.acc')

      write(6,'(''Enter p,theta,phi,id and flag: '',$)')
      read(5,*) p,theta,phi,id,flag
c
      open(unit=11,file='p.dat',form='formatted')
      open(unit=12,file='theta.dat',form='formatted')
      open(unit=13,file='phi.dat',form='formatted')
      do i=0,8*xdim
         pv = pmin+i*dx/8.
         val = getHadesAcceptance(id,pv,theta,phi,flag)
         write(11,*) pv,val
      end do
      close(11)
c
      do j=0,8*ydim
         thv = thmin+j*dy/8.
         val = getHadesAcceptance(id,p,thv,phi,flag)
         write(12,*) thv,val
      end do
      close(12)
c
      do k=0,8*zdim
         phv = phmin+k*dz/8.
         val = getHadesAcceptance(id,p,theta,phv,flag)
         write(13,*) phv,val
      end do
      close(13)
c
      stop
      end
