/**
 * @file   hdstutilities.cc
 * @author Simon Lang
 * @date   Fri May  5 15:58:45 2006
 *
 * @brief  This Class provides Utility Functions for a DST Production
 *
 * $Id: hdstutilities.cc,v 1.6 2008-05-09 16:08:19 halo Exp $
 *
 */

// STL headers
#include <cstdlib>
#include <cstdio>
#include <cstring>

// Root headers
#include "TString.h"
#include "TSystem.h"

// Hydra headers
#include "hdstutilities.h"

using namespace std;

///_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//  Utility Functions for DST Productions
//
//  Actually this class serves as a namespace for some functions which might
//  be helpful for writting a program and/or marco which does a DST production
//  from any kind of data.
//
////////////////////////////////////////////////////////////////////////////////
ClassImp(HDstUtilities)


    Bool_t HDstUtilities::readCmdLineArgsFromFile(const Char_t* filename,
						  Int_t* argc, Char_t*** argv)
{
    // This function parses a text file and save the "key = value" pairs
    // found on the lines of that file in a structure which looks exactly
    // like the one filled with the command line arguments of a program.
    // A file might look like that:
    //
    // ########################################
    // #
    // #  Example config file
    // #
    // ########################################
    //
    // # a comment
    //    # another comment
    //
    // family-name = lang
    // name=simon
    // institute = GSI  # in Darmstadt
    //
    // telephone  = 06519 / 71-2914
    //    fax= "06159 / 71-2989"
    // --email = 's.lang@gsi.de'
    //
    // The corresponding command line would look like this,
    // filename --family-name=lang --name=simon --telephone="06159 / 71-2914"...
    // which is what the getopt_long() function expects. Here, "filename"
    // is the name of the test file being evaluated.
    // The function fills argc and argv with the data and returns true on
    // success. It is written in plain C (except the return type),
    // because it's lazy author wants to use it in other C programs as well.

    FILE* file                          = NULL;
    Int_t   line_num                      = 0;
    Char_t  line[MAX_LINE_LENGTH]         = { '\0' };
    Char_t  argument[MAX_LINE_LENGTH + 2] = { '\0' };
    Char_t* line_end                      = NULL;
    Char_t* option_begin                  = NULL;
    Char_t* option_end                    = NULL;
    Char_t* value_begin                   = NULL;
    Char_t* value_end                     = NULL;

    // save the filename as the first argument - like the program name is
    // saved as the first argument on command line
    *argc      = 1;
    *argv      = (Char_t**)malloc( sizeof(Char_t*) );
    (*argv)[0] = strdup( filename );

    if ((file = fopen( filename, "r" )) == NULL) {
	fprintf( stderr, "==> ERROR: Could not open input file %s!\n", filename);
	return kFALSE;
    }

    while (fgets( line, MAX_LINE_LENGTH, file ) != NULL)
    {
	line_num++;

	// remove trailing newline
	if ((line_end = strrchr( line, '\n' )) != NULL) {
	    *line_end = '\0';
	}

	// skip lines which contain just whitspaces and comments
	if (line[strspn( line, " \t" )] == '#'  ||
	    line[strspn( line, " \t" )] == '\0'   ) {
	    continue;
	}

	// find end of line, skipping trailing comments
	if ((line_end = strchr( line, '#' )) == NULL) {
	    line_end = line + strlen( line );
	}
	*line_end = '\0';

	// isolate option
	option_begin = line + strspn( line, " \t-" );
	option_end   = option_begin + strcspn( option_begin, " =" );

	// isolate value
	value_begin = option_end + strspn( option_end, " =\"'" );
	for (value_end = line_end - 1;
	     *value_end == ' '  ||  *value_end == '\t'  ||
	     *value_end == '"'  ||  *value_end == '\'';
	     value_end--);
	value_end++;

	// check consistency and cut line
	if (option_begin >= line         &&  option_begin < option_end  &&
	    option_end   <  value_begin  &&  value_begin  < value_end   &&
	    value_end    <= line_end) {
	    *option_end = '\0';
	    *value_end  = '\0';
	} else {
	    fprintf( stderr,"==> ERROR: Wrong syntax in line %d of file %s!\n",line_num, filename );
	    fclose( file );
	    return kFALSE;
	}

	// compile pseudo command line argument
	*argument = '\0';
	strcat( argument, "--" );
	strcat( argument, option_begin );
	strcat( argument, "=" );
	strcat( argument, value_begin );

	// save the result
	(*argc)++;
	*argv = (Char_t**)realloc( *argv, *argc * sizeof(Char_t*) );
	(*argv)[*argc - 1] = strdup( argument );
    }

    if (feof( file )) {
	fclose( file );
	return kTRUE;
    } else {
	fclose( file );
	fprintf( stderr, "==> ERROR: Could not read from file %s\n", filename );
	return kFALSE;
    }
}



void HDstUtilities::str2CmdLineArgs(const Char_t* input, Int_t* argc, Char_t*** argv)
{
    // This function converts a string which looks like a command line
    // into the related array of arguments as it is usually passed to the
    // main() function of a program. Results are stored in argc and argv.
    // The first string (usually the name of the executable) is set to
    // "from-string" here.
    // Use (double-) quotes to protect arguments containing white spaces
    // from being cut.
    // This function is written in plain C because it's lazy author wants to
    // use it in other C programs as well.

    const Char_t* source;
    Char_t*       dest;
    Char_t*       argument;
    size_t      length;
    size_t      size;

    // set first argument to "from-string"
    *argc      = 1;
    *argv      = (Char_t**)malloc( sizeof(Char_t*) );
    (*argv)[0] = strdup( "from-string" );

    // create a buffer which has the size of the input string
    length   = 0;
    size     = strlen( input );
    argument = (Char_t*)calloc( size + 1, sizeof(char) );
    source   = input + strspn( input, " \t"  );   // skip leading white spaces
    dest     = argument;

    while (source < input + size)
    {
	if (*source == '\'') {
	    // copy the whole quoted substring to destination
	    length = strcspn( source + 1, "'" );
	    strncpy( dest, source + 1, length );
	    source += length + 2;
	    dest   += length;
	} else if (*source == '"') {
	    // copy the whole Double_t quoted substring to destination
	    length = strcspn( source + 1, "\"" );
	    strncpy( dest, source + 1, length );
	    source += length + 2;
	    dest   += length;
	} else if (*source == ' '  ||  *source == '\t') {
	    *dest = '\0';
	    dest  = argument;

	    // skip embedded or trailing white spaces
	    source += strspn( source + 1, " \t" ) + 1;

	    // save the current argument
	    (*argc)++;
	    *argv = (Char_t**)realloc( *argv, *argc * sizeof(Char_t*) );
	    (*argv)[*argc - 1] = strdup( argument );
	} else {
	    *dest = *source;
	    source++;
	    dest++;
	}
    }

    // save possibly the last argument
    if (dest != argument) {
	*dest = '\0';
	(*argc)++;
	*argv = (Char_t**)realloc( *argv, *argc * sizeof(Char_t*) );
	(*argv)[*argc - 1] = strdup( argument );
    }

    free( argument );
}



void HDstUtilities::freeCmdLineArgs(Int_t argc, Char_t** argv)
{
    // This functions frees the memory used by a command line argument structure

    while (argc > 0) {
	free( argv[--argc] );
    }
    if (argv != NULL) {
	free( argv );
    }
}



Char_t* HDstUtilities::extractArg(Int_t* argc, Char_t*** argv, const Char_t* arg)
{
    // This function parses a list of command line arguments (saved in
    // the structure referenced by 'argc' and 'argv'), creates a new list
    // without the argument specified by 'arg' and returns this argument's
    // name or its value (then one must append a ':' to its name).
    // The memory of the returned string must be deleted by the caller of this
    // function. The references 'argc' and 'argv' are changed to the location
    // of the new list, the old one is not changed.

    Char_t*  result    = NULL;
    Char_t** new_argv  = NULL;
    Int_t    new_argc  = 0;
    size_t length    = 0;
    Int_t    found     = 0;
    Int_t    has_value = 0;
    Int_t    i         = 0;

    // has the search argument a value? - then skip the trailing ':'
    length = strlen( arg );
    if (*(arg + length - 1) == ':') {
	has_value = 1;
	length--;
    }

    // loop over the list and create a new argument list without the first
    // appearance of the search argument
    for (i = 0; i < *argc; i++) {
	if (strncmp( (*argv)[i], arg, length ) == 0  &&  !found)
	{
	    if (has_value  &&  i + 1 < *argc) {
		result = strdup( (*argv)[i + 1] );
		found  = 1;
		i++;
	    } else {
		result = strdup( arg );
		found  = 1;
	    }
	} else {
	    new_argc++;
	    new_argv = (Char_t**)realloc( new_argv, new_argc * sizeof(Char_t*) );
	    new_argv[new_argc - 1] = strdup( (*argv)[i] );
	}
    }

    *argc = new_argc;
    *argv = new_argv;
    return result;
}


TString HDstUtilities::GetFileStem(const TString& dir, const TString& file)
{
    // This function return "dir/file" without the filename extension of 'file'.
    // "file" can be a list of space separated filenames (including paths),
    // then the first one is taken. Paths are stripped from 'file', anyway. If
    // 'dir' is a null string, then just the stem of 'file' is returned.

    TString stem = file;

    // extract first filename
    Ssiz_t first_name_length = stem.Index( " ", 0 );
    if (first_name_length != kNPOS)  stem.Resize( first_name_length );
    stem = gSystem->BaseName( stem.Data() );

    // cut off filename extension
    Ssiz_t pos     = -1;
    Ssiz_t ext_pos = kNPOS;
    while (1) {
	if ((pos = stem.Index( ".", pos + 1 )) == kNPOS)  break;
	ext_pos = pos;
    }
    if (ext_pos != kNPOS) stem.Resize( ext_pos );

    // add new path
    if (!dir.IsNull()) 	stem.Prepend( dir + "/" );

    return stem;
}
