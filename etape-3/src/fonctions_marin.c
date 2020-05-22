

#include <stdio.h>
#include <stdlib.h>
#include <magic.h>

#include "api.h"
#include "utils.h"
#include "fonctions_marin.h"


/*
	Fonction qui renvoie 0 si deux headers de même type sont présents, 1 sinon
*/

int are_unique_headers(void *root)
{
	int to_return = true;
	int size = 0;

	int index_header;
	char *headers_list[] = {"Connection_header", "Content_Length_header", "Content_Type_header", "Cookie_header", "Transfer_Encoding_header", "Expect_header", "Host_header", "Accept_header", "Accept_Charset_header", "Accept_Encoding_header", "Accept_Language_header", "Referer_header", "User_Agent_header"};
	char *header_possible;
	_Token *head, *current_header;

	for (index_header = 0; (to_return == true && index_header < 13); index_header++)
	{
		header_possible = headers_list[index_header];
		head = searchTree(root, header_possible);
		current_header = head;
		// current_header = searchTree(root, "Host__________________header");
		
		while(current_header != NULL)
		{
			size++;
			current_header = current_header->next;
		}
		// size peut valoir 0 s'il n'y a pas le header dans le message http !!!
		if (size != 0 && size != 1)
		{
			printf("\n\n Size invalide pour le header %s : %d headers trouvés. \n\n", header_possible, size);
			to_return = false;
		}
		size = 0;

		purgeElement(&head);
	}

	return to_return;
}


/*
	Fonction qui renvoie 1 si la start-line se termine par HTTP/1.0  OU
	si la start-line se termine par HTTP/1.1 ET un unique host-header est présent ; 0 sinon
*/

/*
int is_http_version_ok(void *root)
{
	int to_return = 0;
	int is_found_host_header = 0;
	int length;

	_Token *http_version;
	_Token *host_header;

	host_header = searchTree(root, "Host_header");
	// host_header = searchTree(root, "Host__________________header");
	if (host_header != NULL && host_header->next == NULL) is_found_host_header = 1;
	printf("\n\n Is found host header : %d \n\n", is_found_host_header);


	http_version = searchTree(root, "HTTP_version");
	// http_version = searchTree(root, "HTTP__________________version");
	printf("\n\n HTTP version found : %d \n\n", http_version == NULL);

	if (http_version != NULL)
	{
		// Récupérer ici ce que contient la http_version pour trouver la version HTTP
		char *http_version_string = getElementValue(http_version->node, &length);
		
		if (http_version_string != NULL)
		{
			printf("\n\n Ligne récupérée : %s. de longueur %d \n\n", http_version_string, length);

			string my_string = {http_version_string, length};
			// Comparer cette version HTTP à ce qui est attendu
			to_return = (compare_strings(&my_string, "HTTP/1.0") || (compare_strings(&my_string, "HTTP/1.1") && is_found_host_header) );
		}
		else printf("\n\n LIGNE NULLE \n\n");
	}
	else printf("\n\n HTTP VERSION NULLE \n\n");

	purgeElement(&host_header);
	purgeElement(&http_version);
	
	return to_return;
}
*/



// Les deux strings existent forcément, mais c'est leur champ ->base qui peut être null
int is_http_version_ok(string *http_version, string *host_header)
{
	int to_return = true;
	int is_found_host_header = false;

	//  && host_header->next == NULL mais un type string n'a pas de ->next 	
	if (host_header->base != NULL) is_found_host_header = true;

	// http_version->base existe forcément sinon le parsing crash
	to_return = (compare_strings(http_version, "HTTP/1.0") || (compare_strings(http_version, "HTTP/1.1") && is_found_host_header) );

	return to_return;
}



string* get_mime_type_with_magic(char *filename)
{
	// Crée un cookie qui "contient" l'option MAGIC_MIME_TYPE
	// Evite d'inclure le flag correspondant à chaque utilisation du cookie, entre autre
	magic_t cookie = magic_open(MAGIC_MIME_TYPE);
	string *result = malloc(sizeof(string));

	printf("\n\n ENTREE DANS GET MIME TYPE WITH MAGIC \n\n");

	if (cookie == NULL)
	{
		printf("\n\n Echec lors de la création du magic cookie. \n\n");
		magic_close(cookie);
		exit(EXIT_FAILURE);
	}
	
	int is_load = magic_load(cookie, "/usr/share/misc/magic.mgc");
	if (is_load == -1)
	{
		printf("\n\n Erreur lors du chargement du fichier /usr/share/misc/magic.mgc : %s. \n\n", magic_error(cookie));
		exit(EXIT_FAILURE);
	}

	const char *mime_type;
	mime_type = magic_file(cookie, filename);

	if (mime_type == NULL)
	{
		printf("\n\n Erreur lors de la récupération des informations sur le fichier donné avec magic_file : %s. \n\n", magic_error(cookie));
		magic_close(cookie);
		exit(EXIT_FAILURE);
	}

	copy_to_string(mime_type, result);
	// Efface toutes les données en mémoire associées au cookie. Oui, TOUTES.
	magic_close(cookie);

	return result;
}


string* get_mime_type(string *filename)
{
	string *to_return = malloc(sizeof(string*));
	//string *to_return;
	int index = 0;
	while (filename->length - index > 0 && filename->base[filename->length -1 - index] != '.')
	{
		//printf("\n %d %c \n", index, filename->base[filename->length -2 - index]);
		index++;
	}
	index--;
	//printf("\n Extension : %s.\n Taille de l'extension : %d \n\n", filename->base + filename->length-1 - index, index);

	if (filename->length - index == 0)
	{
		to_return = get_mime_type_with_magic(filename->base);
	}
	else
	{
		//printf("\n\n ENTREE DANS LE ELSE \n\n");
		//printf("\n\n %s. %d, %d \t %d %c \n\n", filename->base, filename->length, index, filename->length - index, *(filename->base + filename->length - index));
		string *extension = malloc(sizeof(string*));
		extension->base = malloc(8*sizeof(char));
		
		//printf("\n\n DEUXIEME MALLOC REUSSI \n\n");
		//for (int index_tmp = filename->length - index + 1; index_tmp < filename->length; index_tmp++) (extension->base)[index_tmp - filename->length + index -1] = (filename->base)[index_tmp];
		for (int index_tmp = filename->length-1 - index; index_tmp < filename->length; index_tmp++)
		{
			//printf("\n Index tmp : %d ; caractère correspondant : %c ; écrit à la position %d \n", index_tmp, *(filename->base + index_tmp), index_tmp - filename->length+1 + index);
			*(extension->base + index_tmp - filename->length+1 + index) = *(filename->base + index_tmp);
		}
		//printf("\n\n UNTIL HERE IT IS OK 0 \n\n");
		//extension->base = *(filename->base + filename->length - index + 1); // + 1 pour se mettre juste après le .
		extension->length = index;
		//printf("\n\n UNTIL HERE IT IS OK 1 \n\n");

		char woff2[] = "woff2";
		char xhtml[] = "xhtml";
		
		char *extension_de_longueur_cinq[2];
		extension_de_longueur_cinq[0] = woff2;
		extension_de_longueur_cinq[1] = xhtml;

		//printf("\n\n UNTIL HERE IT IS OK 2 \n\n");

		char docx[] = "docx";
		char epub[] = "epub";
		char html[] = "html";
		char jpeg[] = "jpeg";
		char json[] = "json";
		char midi[] = "midi";
		char mpeg[] = "mpeg";
		char mpkg[] = "mpkg";
		char pptx[] = "pptx";
		char tiff[] = "tiff";
		char weba[] = "weba";
		char webm[] = "webm";
		char webp[] = "webp";
		char woff[] = "woff";
		char xlsx[] = "xlsx";
		char troisgp2[] = "3gp2"; // en c les variables ne peuvent pas commencer par un chiffre, comme 3, gg wp
		
		char *extension_de_longueur_quatre[16];
		extension_de_longueur_quatre[0] = docx;
		extension_de_longueur_quatre[1] = epub;
		extension_de_longueur_quatre[2] = html;
		extension_de_longueur_quatre[3] = jpeg;
		extension_de_longueur_quatre[4] = json;
		extension_de_longueur_quatre[5] = midi;
		extension_de_longueur_quatre[6] = mpeg;
		extension_de_longueur_quatre[7] = mpkg;
		extension_de_longueur_quatre[8] = pptx;
		extension_de_longueur_quatre[9] = tiff;
		extension_de_longueur_quatre[10] = weba;
		extension_de_longueur_quatre[11] = webm;
		extension_de_longueur_quatre[12] = webp;
		extension_de_longueur_quatre[13] = woff;
		extension_de_longueur_quatre[14] = xlsx;
		extension_de_longueur_quatre[15] = troisgp2;

		char aac[] = "aac";
		char abw[] = "abw";
		char arc[] = "arc";
		char avi[] = "avi";
		char azw[] = "azw";
		char bin[] = "bin";
		char bz2[] = "bz2";
		char csh[] = "csh";
		char css[] = "css";
		char csv[] = "csv";
		char doc[] = "doc";
		char eot[] = "eot";
		char gif[] = "gif";
		char htm[] = "htm";
		char ico[] = "ico";
		char ics[] = "ics";
		char jar[] = "jar";
		char jpg[] = "jpg";
		char mid[] = "mid";
		char odp[] = "odp";
		char ods[] = "ods";
		char odt[] = "odt";
		char oga[] = "oga";
		char ogv[] = "ogv";
		char ogx[] = "ogx";
		char otf[] = "otf";
		char png[] = "png";
		char pdf[] = "pdf";
		char ppt[] = "ppt";
		char rar[] = "rar";
		char rtf[] = "rtf";
		char svg[] = "svg";
		char swf[] = "swf";
		char tar[] = "tar";
		char tif[] = "tif";
		char ttf[] = "ttf";
		char vsd[] = "vsd";
		char wav[] = "wav";
		char xls[] = "xls";
		char xml[] = "xml";
		char xul[] = "xul";
		char zip[] = "zip";
		char troisgp[] = "3gp"; // en c les variables ne peuvent pas commencer par un chiffre, comme 3, gg wp

		char *extension_de_longueur_trois[43];
		extension_de_longueur_trois[0] = aac;
		extension_de_longueur_trois[1] = abw;
		extension_de_longueur_trois[2] = arc;
		extension_de_longueur_trois[3] = avi;
		extension_de_longueur_trois[4] = azw;
		extension_de_longueur_trois[5] = bin;
		extension_de_longueur_trois[6] = bz2;
		extension_de_longueur_trois[7] = csh;
		extension_de_longueur_trois[8] = css;
		extension_de_longueur_trois[9] = csv;
		extension_de_longueur_trois[10] = doc;
		extension_de_longueur_trois[11] = eot;
		extension_de_longueur_trois[12] = gif;
		extension_de_longueur_trois[13] = htm;
		extension_de_longueur_trois[14] = ico;
		extension_de_longueur_trois[15] = ics;
		extension_de_longueur_trois[16] = jar;
		extension_de_longueur_trois[17] = jpg;
		extension_de_longueur_trois[18] = mid;
		extension_de_longueur_trois[19] = odp;
		extension_de_longueur_trois[20] = ods;
		extension_de_longueur_trois[21] = odt;
		extension_de_longueur_trois[22] = oga;
		extension_de_longueur_trois[23] = ogv;
		extension_de_longueur_trois[24] = ogx;
		extension_de_longueur_trois[25] = otf;
		extension_de_longueur_trois[26] = png;
		extension_de_longueur_trois[27] = pdf;
		extension_de_longueur_trois[28] = ppt;
		extension_de_longueur_trois[29] = rar;
		extension_de_longueur_trois[30] = rtf;
		extension_de_longueur_trois[31] = svg;
		extension_de_longueur_trois[32] = swf;
		extension_de_longueur_trois[33] = tar;
		extension_de_longueur_trois[34] = tif;
		extension_de_longueur_trois[35] = ttf;
		extension_de_longueur_trois[36] = vsd;
		extension_de_longueur_trois[37] = wav;
		extension_de_longueur_trois[38] = xls;
		extension_de_longueur_trois[39] = xml;
		extension_de_longueur_trois[40] = xul;
		extension_de_longueur_trois[41] = zip;
		extension_de_longueur_trois[42] = troisgp;

		char bz[] = "bz";
		char js[] = "js";
		char sh[] = "sh";
		char ts[] = "ts";
		char septz[] = "7z";

		char *extension_de_longueur_deux[5];
		extension_de_longueur_deux[0] = bz;
		extension_de_longueur_deux[1] = js;
		extension_de_longueur_deux[2] = sh;
		extension_de_longueur_deux[3] = ts;
		extension_de_longueur_deux[4] = septz;





		char woff2_mime[] = "font/woff2";
		char xhtml_mime[] = "application/xhtml+xml";
		
		char *extension_de_longueur_cinq_mime[2];
		extension_de_longueur_cinq_mime[0] = woff2_mime;
		extension_de_longueur_cinq_mime[1] = xhtml_mime;

		char docx_mime[] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
		char epub_mime[] = "application/epub+zip";
		char html_mime[] = "text/html";
		char jpeg_mime[] = "image/jpeg";
		char json_mime[] = "application/json";
		char midi_mime[] = "audio/midi";
		char mpeg_mime[] = "video/mpeg";
		char mpkg_mime[] = "application/vnd.apple.installer+xml";
		char pptx_mime[] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
		char tiff_mime[] = "image/tiff";
		char weba_mime[] = "audio/webm";
		char webm_mime[] = "video/webm";
		char webp_mime[] = "image/webp";
		char woff_mime[] = "font/woff";
		char xlsx_mime[] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
		char troisgp2_mime[] = "video/3gpp2";

		char *extension_de_longueur_quatre_mime[16];
		extension_de_longueur_quatre_mime[0] = docx_mime;
		extension_de_longueur_quatre_mime[1] = epub_mime;
		extension_de_longueur_quatre_mime[2] = html_mime;
		extension_de_longueur_quatre_mime[3] = jpeg_mime;
		extension_de_longueur_quatre_mime[4] = json_mime;
		extension_de_longueur_quatre_mime[5] = midi_mime;
		extension_de_longueur_quatre_mime[6] = mpeg_mime;
		extension_de_longueur_quatre_mime[7] = mpkg_mime;
		extension_de_longueur_quatre_mime[8] = pptx_mime;
		extension_de_longueur_quatre_mime[9] = tiff_mime;
		extension_de_longueur_quatre_mime[10] = weba_mime;
		extension_de_longueur_quatre_mime[11] = webm_mime;
		extension_de_longueur_quatre_mime[12] = webp_mime;
		extension_de_longueur_quatre_mime[13] = woff_mime;
		extension_de_longueur_quatre_mime[14] = xlsx_mime;
		extension_de_longueur_quatre_mime[15] = troisgp2_mime;


		char aac_mime[] = "audio/aac";
		char abw_mime[] = "application/x-abiword";
		char arc_mime[] = "application/octet-stream";
		char avi_mime[] = "video/x-msvideo";
		char azw_mime[] = "application/vnd.amazon.ebook";
		char bin_mime[] = "application/octet-stream";
		char bz2_mime[] = "application/x-bzip2";
		char csh_mime[] = "application/x-csh";
		char css_mime[] = "text/css";
		char csv_mime[] = "text/csv";
		char doc_mime[] = "application/msword";
		char eot_mime[] = "application/vnd.ms-fontobject";
		char gif_mime[] = "image/gif";
		char htm_mime[] = "text/html";
		char ico_mime[] = "image/x-icon";
		char ics_mime[] = "text/calendar";
		char jar_mime[] = "application/java-archive";
		char jpg_mime[] = "image/jpeg";
		char mid_mime[] = "audio/midi";
		char odp_mime[] = "application/vnd.oasis.opendocument.presentation";
		char ods_mime[] = "application/vnd.oasis.opendocument.spreadsheet";
		char odt_mime[] = "application/vnd.oasis.opendocument.text";
		char oga_mime[] = "audio/ogg";
		char ogv_mime[] = "video/ogg";
		char ogx_mime[] = "application/ogg";
		char otf_mime[] = "font/otf";
		char png_mime[] = "image/png";
		char pdf_mime[] = "application/pdf";
		char ppt_mime[] = "application/vnd.ms-powerpoint";
		char rar_mime[] = "application/x-rar-compressed";
		char rtf_mime[] = "application/rtf";
		char svg_mime[] = "image/svg+xml";
		char swf_mime[] = "application/x-shockwave-flash";
		char tar_mime[] = "application/x-tar";
		char tif_mime[] = "image/tiff";
		char ttf_mime[] = "font/ttf";
		char vsd_mime[] = "application/vnd.visio";
		char wav_mime[] = "audio/x-wav";
		char xls_mime[] = "application/vnd.ms-excel";
		char xml_mime[] = "application/xml";
		char xul_mime[] = "application/vnd.mozilla.xul+xml";
		char zip_mime[] = "application/zip";
		char troisgp_mime[] = "video/3gpp";


		char *extension_de_longueur_trois_mime[43];
		extension_de_longueur_trois_mime[0] = aac_mime;
		extension_de_longueur_trois_mime[1] = abw_mime;
		extension_de_longueur_trois_mime[2] = arc_mime;
		extension_de_longueur_trois_mime[3] = avi_mime;
		extension_de_longueur_trois_mime[4] = azw_mime;
		extension_de_longueur_trois_mime[5] = bin_mime;
		extension_de_longueur_trois_mime[6] = bz2_mime;
		extension_de_longueur_trois_mime[7] = csh_mime;
		extension_de_longueur_trois_mime[8] = css_mime;
		extension_de_longueur_trois_mime[9] = csv_mime;
		extension_de_longueur_trois_mime[10] = doc_mime;
		extension_de_longueur_trois_mime[11] = eot_mime;
		extension_de_longueur_trois_mime[12] = gif_mime;
		extension_de_longueur_trois_mime[13] = htm_mime;
		extension_de_longueur_trois_mime[14] = ico_mime;
		extension_de_longueur_trois_mime[15] = ics_mime;
		extension_de_longueur_trois_mime[16] = jar_mime;
		extension_de_longueur_trois_mime[17] = jpg_mime;
		extension_de_longueur_trois_mime[18] = mid_mime;
		extension_de_longueur_trois_mime[19] = odp_mime;
		extension_de_longueur_trois_mime[20] = ods_mime;
		extension_de_longueur_trois_mime[21] = odt_mime;
		extension_de_longueur_trois_mime[22] = oga_mime;
		extension_de_longueur_trois_mime[23] = ogv_mime;
		extension_de_longueur_trois_mime[24] = ogx_mime;
		extension_de_longueur_trois_mime[25] = otf_mime;
		extension_de_longueur_trois_mime[26] = png_mime;
		extension_de_longueur_trois_mime[27] = pdf_mime;
		extension_de_longueur_trois_mime[28] = ppt_mime;
		extension_de_longueur_trois_mime[29] = rar_mime;
		extension_de_longueur_trois_mime[30] = rtf_mime;
		extension_de_longueur_trois_mime[31] = svg_mime;
		extension_de_longueur_trois_mime[32] = swf_mime;
		extension_de_longueur_trois_mime[33] = tar_mime;
		extension_de_longueur_trois_mime[34] = tif_mime;
		extension_de_longueur_trois_mime[35] = ttf_mime;
		extension_de_longueur_trois_mime[36] = vsd_mime;
		extension_de_longueur_trois_mime[37] = wav_mime;
		extension_de_longueur_trois_mime[38] = xls_mime;
		extension_de_longueur_trois_mime[39] = xml_mime;
		extension_de_longueur_trois_mime[40] = xul_mime;
		extension_de_longueur_trois_mime[41] = zip_mime;
		extension_de_longueur_trois_mime[42] = troisgp_mime;

		char bz_mime[] = "application/x-bzip";
		char js_mime[] = "application/javascript";
		char sh_mime[] = "application/x-sh";
		char ts_mime[] = "application/typescript";
		char septz_mime[] = "application/x-7z-compressed";

		char *extension_de_longueur_deux_mime[5];
		extension_de_longueur_deux_mime[0] = bz_mime;
		extension_de_longueur_deux_mime[1] = js_mime;
		extension_de_longueur_deux_mime[2] = sh_mime;
		extension_de_longueur_deux_mime[3] = ts_mime;
		extension_de_longueur_deux_mime[4] = septz_mime;



		//printf("\n\n UNTIL HERE IT IS OK 10 \n\n");
		//printf("\n Extension : %s.\n Taille de l'extension : %d \n\n", extension->base, extension->length);

		int index_extension = 0;
		int is_found = 0;

		if (extension->length == 2)
		{
			//printf("\n\n TAILLE DE L EXTENSION == 2 \n\n");
			while (is_found == 0 && index_extension < 5)
			{
				is_found = compare_strings(extension, extension_de_longueur_deux[index_extension]);
				if (is_found == 0) index_extension++;
				else copy_to_string(extension_de_longueur_deux_mime[index_extension], to_return);
			}
		}
		
		else if (extension->length == 3)
		{
			//printf("\n\n TAILLE DE L EXTENSION == 3 \n\n");
			while (is_found == 0 && index_extension < 43)
			{
				is_found = compare_strings(extension, extension_de_longueur_trois[index_extension]);
				if (is_found == 0) index_extension++;
				else copy_to_string(extension_de_longueur_trois_mime[index_extension], to_return);
			}
		}

		else if (extension->length == 4)
		{
			//printf("\n\n TAILLE DE L EXTENSION == 4 \n\n");
			while(is_found == 0 && index_extension < 16)
			{
				is_found = compare_strings(extension, extension_de_longueur_quatre[index_extension]);
				//printf("\n\n Is found %d : %d \n\n", index_extension, is_found);
				if (is_found == 0) index_extension++;
				else
				{
					//printf("\n\n Entrée dans copy to string - %s. \n\n", extension_de_longueur_quatre_mime[index_extension]);
					copy_to_string(extension_de_longueur_quatre_mime[index_extension], to_return);
				}
			}
		}

		else if (extension->length == 5)
		{
			//printf("\n\n TAILLE DE L EXTENSION == 5 \n\n");
			while(is_found == 0 && index_extension < 2)
			{
				is_found = compare_strings(extension, extension_de_longueur_cinq[index_extension]);
				if (is_found == 0) index_extension++;
				else copy_to_string(extension_de_longueur_cinq_mime[index_extension], to_return);
			}
		}

		else
		{
			to_return = get_mime_type_with_magic(filename->base);
		}

		free(extension->base);
		free(extension);
	}

	printf("\n\n UNTIL HERE IT IS OK 100 \n\n");
	printf("\n\n Type du légendaire Mr Mime : \n %s. de longueur %d \n\n", to_return->base, to_return->length);


	return to_return;
}