import * as functions from 'firebase-functions';
import * as admin from 'firebase-admin';
admin.initializeApp();

// // Start writing Firebase Functions
// // https://firebase.google.com/docs/functions/typescript
//
// export const helloWorld = functions.https.onRequest((request, response) => {
//  response.send("Hello from Firebase!");
// });

const firestore = admin.firestore();

export const updateTemp = functions.https.onRequest(async (request, response) => {
	console.log(request.body);
	await firestore.collection("foods").doc(request.body.id).update({
		temperature: request.body.temperature
	});
	response.sendStatus(200);
});
