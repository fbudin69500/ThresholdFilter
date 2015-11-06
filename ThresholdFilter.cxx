#include "ThresholdFilterCLP.h"
#include "itkImageFileWriter.h"
#include <itkImage.h>
#include <itkImageFileReader.h>


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

template< class T , class ThresholdType >
void Thresholding( double lowerThreshold ,
                   double upperThreshold ,
                   T* ptr ,
                   long length ,
                   bool keepValues ,
                   double outsideValue ,
                   double insideValue
                 )
{
  if( keepValues )
  {
    for( long i = 0 ; i < length ; i++ )
    {
      *( ptr + i ) = ( ( static_cast< ThresholdType > ( *( ptr + i ) ) < static_cast< ThresholdType >( lowerThreshold )
                     || static_cast< ThresholdType > ( *( ptr + i ) ) > static_cast< ThresholdType >( upperThreshold )
                     ) ? 0 : *( ptr + i ) ) ;
    }
  }
  else
  {
    for( long i = 0 ; i < length ; i++ )
    {
      *( ptr + i ) = ( ( static_cast< ThresholdType > ( *( ptr + i ) ) < static_cast< ThresholdType >( lowerThreshold )
                     || static_cast< ThresholdType > ( *( ptr + i ) ) > static_cast< ThresholdType >( upperThreshold )
                     ) ? static_cast< T >( outsideValue ) : static_cast< T >( insideValue ) ) ;
    }
  }
}


template<class T> int DoIt( int argc, char * argv[] )
{
    /////////////////   Get arguments using GenerateCLP parser to get all the arguments  ///////////////////////
    PARSE_ARGS ;
    typedef itk::Image< T, 3 >   InputImageType ;
    /////////////////   Load input file    ////////////////////////////////////////////////
    typedef itk::ImageFileReader< InputImageType > ImageReaderType ;
    typename ImageReaderType::Pointer reader = ImageReaderType::New() ;
    reader->SetFileName( inputVolume ) ;
    reader->Update() ;
    typename InputImageType::SizeType size ;
    size = reader->GetOutput()->GetLargestPossibleRegion().GetSize() ;
    /////////////////// Threshold Filter    //////////////////////////////////////////
    if( noLowerThreshold )
    {
        lowerThreshold = std::numeric_limits< T >::min() ;
    }
    if( noUpperThreshold )
    {
        upperThreshold = std::numeric_limits< T >::max() ;
    }
//    std::cout<<"lower: "<<lowerThreshold<<std::endl ;
//    std::cout<<"upper: "<<upperThreshold<<std::endl;
    T* ptr = reader->GetOutput()->GetBufferPointer() ;
    long length = size[ 0 ] * size[ 1 ] * size[ 2 ] ;
    //If threshold is of type float    
    if( !thresholdType.compare( "float" ) )
    {
//      std::cout<<"double"<<std::endl ;
      Thresholding< T , double >( lowerThreshold , upperThreshold , ptr , length , keepValues , outsideValue , insideValue ) ;
    }
    else //if threshold is of type int
    {
//      std::cout<<"integer"<<std::endl;
      Thresholding< T , int >( lowerThreshold , upperThreshold , ptr , length , keepValues , outsideValue , insideValue ) ;
    }
    ///////////////////    Save output volume   ////////////////////////////////////////////////
    typedef itk::ImageFileWriter< InputImageType > ImageWriterType ;
    typename ImageWriterType::Pointer writer = ImageWriterType::New() ;
    writer->SetFileName( outputVolume ) ;
    writer->SetInput( reader->GetOutput() ) ;
    writer->SetUseCompression( true ) ;
    writer->Update() ;
    return EXIT_SUCCESS ;
}

void GetImageType( std::string fileName,
                   itk::ImageIOBase::IOPixelType &pixelType ,
                   itk::ImageIOBase::IOComponentType &componentType
                 )
{
  typedef itk::Image< unsigned char , 3 > ImageType ;
  itk::ImageFileReader< ImageType >::Pointer imageReader 
              = itk::ImageFileReader< ImageType >::New() ;
  imageReader->SetFileName( fileName.c_str() ) ;
  imageReader->UpdateOutputInformation() ;
  pixelType = imageReader->GetImageIO()->GetPixelType() ;
  componentType = imageReader->GetImageIO()->GetComponentType() ;
}



int TemplateInputVolume( std::string inputVolume , int argc , char * argv[] )
{
  itk::ImageIOBase::IOPixelType pixelType ;
  itk::ImageIOBase::IOComponentType componentType ;
  try
  {
    GetImageType ( inputVolume , pixelType , componentType ) ;
    // This filter handles all image component types
    switch( componentType )
    {
      case itk::ImageIOBase::UCHAR:
//        std::cout<<"uchar"<<std::endl;
        return DoIt< unsigned char >( argc , argv ) ;
        break ;
      case itk::ImageIOBase::CHAR:
//        std::cout<<"char"<<std::endl;
        return DoIt< char >( argc , argv ) ;
        break ;
      case itk::ImageIOBase::USHORT:
//        std::cout<<"ushort"<<std::endl;
        return DoIt< unsigned short >( argc , argv ) ;
        break ;
      case itk::ImageIOBase::SHORT:
//        std::cout<<"short"<<std::endl;
        return DoIt< short >( argc , argv ) ;
        break ;
      case itk::ImageIOBase::UINT:
//        std::cout<<"uint"<<std::endl;
        return DoIt< unsigned int >( argc , argv ) ;
        break ;
      case itk::ImageIOBase::INT:
//        std::cout<<"int"<<std::endl;
        return DoIt< int >( argc , argv ) ;
        break ;
      case itk::ImageIOBase::ULONG:
//        std::cout<<"ulong"<<std::endl;
        return DoIt< unsigned long >( argc , argv ) ;
        break ;
      case itk::ImageIOBase::LONG:
//        std::cout<<"long"<<std::endl;
        return DoIt< long >( argc , argv ) ;
        break ;
      case itk::ImageIOBase::FLOAT:
//        std::cout<<"float"<<std::endl;
        return DoIt< float >( argc , argv ) ;
        break ;
      case itk::ImageIOBase::DOUBLE:
//        std::cout<<"double"<<std::endl;
        return DoIt< double >( argc , argv ) ;
        break ;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
        default:
//        std::cout << "unknown component type" << std::endl ;
        return EXIT_FAILURE ;
        break ;
    }
  }
  catch( itk::ExceptionObject &excep )
  {
    std::cerr << argv[0] << ": exception caught !" << std::endl ;
    std::cerr << excep << std::endl ;
    return EXIT_FAILURE ;
  }
}


int main( int argc, char * argv[] )
{
  /////////////////   Get arguments using GenerateCLP
  //// parser to get the input volume filename  ///////////////////////
  PARSE_ARGS;
  /////////////////   Read input volume data type and instantiate
  //// the corresponding templated filter  function    ////////////////////
  return TemplateInputVolume( inputVolume , argc , argv );
}
