#!/usr/bin/php
/*
Run this script with the PHP Command Line Interface to validate a 
configuration XML file against its Document Type Definition.
*/
<?php
if ( count( $argv ) < 2 )
{
	die( "Usage: validate.php [xml file]\n" );
}

$dom = new DOMDocument;
$dom->load( $argv[1] );
if ( $dom->validate() )
	echo "Validation passed!\n";
?>
