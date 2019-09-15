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

export const addMessage = functions.https.onRequest(async (request, response) => {
	const original = request.query.text;
	await firestore.collection("foods").doc("0").set({
		temprature: original
	});
	response.sendStatus(200);
});
