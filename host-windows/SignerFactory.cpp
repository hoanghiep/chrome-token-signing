/*
* Chrome Token Signing Native Host
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "SignerFactory.h"
#include "CngCapiSigner.h"
#include "Pkcs11Signer.h"
#include "AtrFetcher.h"
#include "PKCS11ModulePath.h"
#include <string>

Signer * SignerFactory::createSigner(const jsonxx::Object &jsonRequest){
	string hashFromStdIn = jsonRequest.get<string>("hash");
	string cert = jsonRequest.get<string>("cert");
	
	AtrFetcher * atrFetcher = new AtrFetcher();
	std::vector<std::string> atrs = atrFetcher->fetchAtr();
	for (int i = 0; i < atrs.size(); i++) {
		//3BFD1800008031FE4553434536302D43443134352D46CD - SafeSign C:\\Windows\\System32\\aetpkss1.dll
		//3BFA1800008031FE45FE654944202F20504B4903 - V�rnik opensc-pkcs11.dll
		if (PKCS11ModulePath::isKnownAtr(atrs[i])) {
			Pkcs11Signer *signer = new Pkcs11Signer(hashFromStdIn, cert);
			signer->setPkcs11ModulePath(PKCS11ModulePath::getModulePath());
			signer->initialize();
			return signer;
		}
	}
	return new CngCapiSigner(hashFromStdIn, cert);
}
