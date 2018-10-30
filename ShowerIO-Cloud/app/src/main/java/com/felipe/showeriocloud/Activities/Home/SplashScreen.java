package com.felipe.showeriocloud.Activities.Home;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.AWSStartupHandler;
import com.amazonaws.mobile.client.AWSStartupResult;
import com.facebook.AccessToken;
import com.felipe.showeriocloud.Activities.Authentication.LoginActivity;
import com.felipe.showeriocloud.Activities.SmartConfig.SearchForDevices;
import com.felipe.showeriocloud.Aws.AwsDynamoDBManager;
import com.felipe.showeriocloud.Aws.CognitoSyncClientManager;
import com.felipe.showeriocloud.Main2Activity;
import com.felipe.showeriocloud.R;


public class SplashScreen extends AppCompatActivity {

    private static int SPLASH_TIME_OUT = 1000;
    private static final String TAG = "SplashScreen";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.activity_home);

        /**
         * Initializes the sync client. This must be call before you can use it.
         */
        CognitoSyncClientManager.init(this);

        AWSMobileClient.getInstance().initialize(this, new AWSStartupHandler() {
            @Override
            public void onComplete(AWSStartupResult awsStartupResult) {

                //Start
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        final AccessToken fbAccessToken = AccessToken.getCurrentAccessToken();
                        if (fbAccessToken != null) {
                            setFacebookSession(fbAccessToken);
                            Thread thread = new Thread(new Runnable() {
                                @Override
                                public void run() {
                                    try {
                                        if (CognitoSyncClientManager.credentialsProvider.getCredentials().getSessionToken().isEmpty()) {
                                            Toast.makeText(SplashScreen.this, "Error in Facebook login ", Toast.LENGTH_LONG).show();
                                            CognitoSyncClientManager.credentialsProvider.clearCredentials();
                                            CognitoSyncClientManager.credentialsProvider.clear();
                                            Intent loginActivity = new Intent(SplashScreen.this, LoginActivity.class);
                                            startActivity(loginActivity);
                                            finish();
                                        } else {
                                            Log.d(TAG, "CognitoSyncClientManger returned a valid token, user is authenticated, changing activity");
                                            initializeAwsServices();
                                            AWSMobileClient.getInstance().setCredentialsProvider(CognitoSyncClientManager.credentialsProvider);
                                            Intent testActivity = new Intent(SplashScreen.this, Main2Activity.class);
                                            startActivity(testActivity);
                                            finish();
                                        }
                                    } catch (Exception e) {
                                        e.printStackTrace();
                                    }
                                }
                            });
                            thread.start();
                        } else {
                            Intent loginActivity = new Intent(SplashScreen.this, LoginActivity.class);
                            startActivity(loginActivity);
                            finish();
                        }


                    }
                }, SPLASH_TIME_OUT);

            }
        }).execute();

    }

    public void initializeAwsServices() {
        //Initializing DynamoDB instances
        AwsDynamoDBManager awsDynamoDBManager = new AwsDynamoDBManager();
        awsDynamoDBManager.initializeDynamoDb();
    }

    private void setFacebookSession(AccessToken accessToken) {
        Log.i(TAG, "facebook token: " + accessToken.getToken());
        CognitoSyncClientManager.addLogins("graph.facebook.com",
                accessToken.getToken());
    }


}
